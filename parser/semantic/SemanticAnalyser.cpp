#include "SemanticAnalyser.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../Program.hpp"
#include "SemanticError.hpp"
#include "../../vm/Context.hpp"
#include "../../vm/standard/NumberSTD.hpp"
#include "../../vm/standard/StringSTD.hpp"
#include "../../vm/standard/ArraySTD.hpp"
#include "../../vm/standard/ObjectSTD.hpp"

using namespace std;

SemanticAnalyser::SemanticAnalyser() {
	program = nullptr;
	in_function = false;
	in_program = false;
	reanalyse = false;
}

SemanticAnalyser::~SemanticAnalyser() {}

void SemanticVar::will_take(SemanticAnalyser* analyser, unsigned pos, const Type& type) {
	if (value != nullptr) {
		bool changed = value->will_take(analyser, pos, type);
		this->type.will_take(pos, type);
		if (changed) {
			analyser->reanalyse = true;
//			cout << "REANALYSE" << endl;
		}
	}
}

void SemanticAnalyser::analyse(Program* program, Context* context) {

	this->program = program;

	// Add context variables
	for (auto var : context->vars) {
		add_global_var(var.first, Type(var.second->getRawType(), Nature::POINTER), nullptr);
	}

	Type op_type = Type(RawType::FUNCTION, Nature::POINTER);
	op_type.setArgumentType(0, Type::INTEGER);
	op_type.setArgumentType(0, Type::INTEGER);
	op_type.setReturnType(Type::INTEGER);
	add_var("+", op_type, nullptr);
	add_var("-", op_type, nullptr);
	add_var("*", op_type, nullptr);
	add_var("/", op_type, nullptr);
	add_var("^", op_type, nullptr);
	add_var("%", op_type, nullptr);

	Type print_type = Type(RawType::FUNCTION, Nature::POINTER);
	print_type.setArgumentType(0, Type::VALUE);
	print_type.setReturnType(Type::STRING);
	add_var("print", print_type, nullptr);

	NumberSTD().include(this, program);
	StringSTD().include(this, program);
	ArraySTD().include(this, program);
	ObjectSTD().include(this, program);

	in_program = true;

	do {
//		cout << "--------" << endl << "Analyse" << endl << "--------" << endl;
		reanalyse = false;
		program->body->analyse(this, Type::POINTER);
	} while (reanalyse);

	program->functions = functions;
	program->global_vars = global_vars;
}

void SemanticAnalyser::enter_function(Function* f) {
	in_function = true;
	local_vars.push_back(map<string, SemanticVar*> {});
	parameters.push_back(map<string, SemanticVar*> {});
	functions_stack.push(f);
}

void SemanticAnalyser::leave_function() {
	in_function = false;
	local_vars.pop_back();
	parameters.pop_back();
	functions_stack.pop();
}

Function* SemanticAnalyser::current_function() const {
	if (functions_stack.empty()) {
		return nullptr;
	}
	return functions_stack.top();
}

SemanticVar* SemanticAnalyser::add_parameter(string name, Type type) {

	SemanticVar* arg = new SemanticVar(VarScope::PARAMETER, type, parameters.back().size(), nullptr, false);
	parameters.back().insert(pair<string, SemanticVar*>(name, arg));
	return arg;
}

SemanticVar* SemanticAnalyser::get_var(Token* name) {
	try {
		return internal_vars.at(name->content);
		} catch (exception& e) {}
	try {
		return global_vars.at(name->content);
	} catch (exception& e) {}
	try {
		if (parameters.size() > 0) {
			return parameters.back().at(name->content);
		}
	} catch (exception& e) {}
	try {
		if (local_vars.size() > 0) {
			return local_vars.back().at(name->content);
		}
	} catch (exception& e) {}
	throw SemanticError(name->line, "Variable « " + name->content + " » is undefined!");
}

SemanticVar* SemanticAnalyser::get_var_direct(string name) {
	try {
		return global_vars.at(name);
	} catch (exception& e) {}
	try {
		if (local_vars.size() > 0) {
			return local_vars.back().at(name);
		}
	} catch (exception& e) {}
	return nullptr;
}

SemanticVar* SemanticAnalyser::add_var(string v, Type type, Value* value) {
	return add_var(v, type, value, false);
}

void SemanticAnalyser::add_global_var(string v, Type type, Value* value) {
	global_vars.insert(pair<string, SemanticVar*>(
		v,
		new SemanticVar(VarScope::GLOBAL, type, 0, value, internal)
	));
}

SemanticVar* SemanticAnalyser::add_var(string v, Type type, Value* value, bool internal) {

//	cout << "add var" << endl;

	if (in_program) {

		if (in_function) {
	//		cout << "local" << endl;
			local_vars.back().insert(pair<string, SemanticVar*>(
				v,
				new SemanticVar(VarScope::LOCAL, type, 0, value, internal)
			));
			return local_vars.back().at(v);
		} else {
	//		cout << "global" << endl;
			global_vars.insert(pair<string, SemanticVar*>(
				v,
				new SemanticVar(VarScope::GLOBAL, type, 0, value, internal)
			));
			return global_vars.at(v);
		}
	} else {
		internal_vars.insert(pair<string, SemanticVar*>(
			v,
			new SemanticVar(VarScope::INTERNAL, type, 0, value, internal)
		));
		return internal_vars.at(v);
	}
}

void SemanticAnalyser::add_function(Function* l) {
	functions.push_back(l);
}

map<string, SemanticVar*>& SemanticAnalyser::get_local_vars() {
	return local_vars.back();
}
