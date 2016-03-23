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

extern LSValue* jit_add(LSValue* x, LSValue* y);
extern LSValue* jit_sub(LSValue* x, LSValue* y);
extern LSValue* jit_mul(LSValue* x, LSValue* y);
extern LSValue* jit_div(LSValue* x, LSValue* y);
extern LSValue* jit_pow(LSValue* x, LSValue* y);
extern LSValue* jit_mod(LSValue* x, LSValue* y);
void SemanticAnalyser::analyse(Program* program, Context* context) {

	this->program = program;

	// Add context variables
	for (auto var : context->vars) {
		add_global_var(new Token(var.first), Type(var.second->getRawType(), Nature::POINTER), nullptr);
	}

	Type op_type = Type(RawType::FUNCTION, Nature::POINTER);
	op_type.setArgumentType(0, Type::POINTER);
	op_type.setArgumentType(1, Type::POINTER);
	op_type.setReturnType(Type::POINTER);
	program->system_vars.insert(pair<string, LSValue*>("+", new LSFunction((void*) &jit_add)));
	add_var(new Token("+"), op_type, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("-", new LSFunction((void*) &jit_sub)));
	add_var(new Token("-"), op_type, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("*", new LSFunction((void*) &jit_mul)));
	add_var(new Token("*"), op_type, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("/", new LSFunction((void*) &jit_div)));
	add_var(new Token("/"), op_type, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("^", new LSFunction((void*) &jit_pow)));
	add_var(new Token("^"), op_type, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("%", new LSFunction((void*) &jit_mod)));
	add_var(new Token("%"), op_type, nullptr);

	Type print_type = Type(RawType::FUNCTION, Nature::POINTER);
	print_type.setArgumentType(0, Type::VALUE);
	print_type.setReturnType(Type::POINTER);
	add_var(new Token("print"), print_type, nullptr);

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

SemanticVar* SemanticAnalyser::add_parameter(Token* v, Type type) {

	SemanticVar* arg = new SemanticVar(VarScope::PARAMETER, type, parameters.back().size(), nullptr);
	parameters.back().insert(pair<string, SemanticVar*>(v->content, arg));
	return arg;
}

SemanticVar* SemanticAnalyser::get_var(Token* v) {
	try {
		return internal_vars.at(v->content);
		} catch (exception& e) {}
	try {
		return global_vars.at(v->content);
	} catch (exception& e) {}
	try {
		if (parameters.size() > 0) {
			return parameters.back().at(v->content);
		}
	} catch (exception& e) {}
	try {
		if (local_vars.size() > 0) {
			return local_vars.back().at(v->content);
		}
	} catch (exception& e) {}
	throw SemanticError(v, "Variable « " + v->content + " » is undefined!");
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

void SemanticAnalyser::add_global_var(Token* v, Type type, Value* value) {
	global_vars.insert(pair<string, SemanticVar*>(
		v->content,
		new SemanticVar(VarScope::GLOBAL, type, 0, value)
	));
}

SemanticVar* SemanticAnalyser::add_var(Token* v, Type type, Value* value) {

//	cout << "add var " << v << endl;

	if (in_program) {

		if (in_function) {
	//		cout << "local" << endl;
			local_vars.back().insert(pair<string, SemanticVar*>(
				v->content,
				new SemanticVar(VarScope::LOCAL, type, 0, value)
			));
			return local_vars.back().at(v->content);
		} else {
//			cout << "global" << endl;

			if (global_vars.find(v->content) != global_vars.end()) {
				throw SemanticError(v, "Variable « " + v->content + " » is already defined!");
			}
			global_vars.insert(pair<string, SemanticVar*>(
				v->content,
				new SemanticVar(VarScope::GLOBAL, type, 0, value)
			));
			return global_vars.at(v->content);
		}
	} else {
		internal_vars.insert(pair<string, SemanticVar*>(
			v->content,
			new SemanticVar(VarScope::INTERNAL, type, 0, value)
		));
		return internal_vars.at(v->content);
	}
}

void SemanticAnalyser::add_function(Function* l) {
	functions.push_back(l);
}

map<string, SemanticVar*>& SemanticAnalyser::get_local_vars() {
	return local_vars.back();
}
