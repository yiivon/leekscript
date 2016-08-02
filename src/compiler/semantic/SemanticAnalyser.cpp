#include "../../compiler/semantic/SemanticAnalyser.hpp"

#include "../../compiler/instruction/ExpressionInstruction.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Context.hpp"
#include "../../vm/standard/NullSTD.hpp"
#include "../../vm/standard/NumberSTD.hpp"
#include "../../vm/standard/BooleanSTD.hpp"
#include "../../vm/standard/StringSTD.hpp"
#include "../../vm/standard/ArraySTD.hpp"
#include "../../vm/standard/ObjectSTD.hpp"
#include "../../vm/standard/SystemSTD.hpp"
#include "../../vm/standard/FunctionSTD.hpp"
#include "../../vm/standard/ClassSTD.hpp"
#include "SemanticException.hpp"
#include "../instruction/VariableDeclaration.hpp"

using namespace std;

namespace ls {

SemanticAnalyser::SemanticAnalyser() {
	program = nullptr;
	in_function = false;
	in_program = false;
	reanalyse = false;
	loops.push(0);
}

SemanticAnalyser::~SemanticAnalyser() {

}

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

void SemanticVar::will_take_element(SemanticAnalyser* analyser, const Type& type) {
	if (value != nullptr) {
		bool changed = value->will_take_element(analyser, type);
		this->type.will_take_element(type);
		if (changed) {
			//analyser->reanalyse = true;
//			cout << "REANALYSE" << endl;
		}
	}
}

void SemanticVar::must_be_pointer(SemanticAnalyser* analyser) {
	if (value != nullptr) {
		bool changed = value->must_be_pointer(analyser);
		this->type.nature = Nature::POINTER;
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

void SemanticAnalyser::analyse(Program* program, Context* context, std::vector<Module*>& modules) {

	this->program = program;

	// Add context variables
	for (auto var : context->vars) {
		add_var(new Token(var.first), Type(var.second->getRawType(), Nature::POINTER), nullptr, nullptr);
	}

	Type op_type = Type(RawType::FUNCTION, Nature::POINTER);
	op_type.setArgumentType(0, Type::POINTER);
	op_type.setArgumentType(1, Type::POINTER);
	op_type.setReturnType(Type::POINTER);
	program->system_vars.insert(pair<string, LSValue*>("+", new LSFunction((void*) &jit_add, 1)));
	add_var(new Token("+"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("-", new LSFunction((void*) &jit_sub, 1)));
	add_var(new Token("-"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("*", new LSFunction((void*) &jit_mul, 1)));
	add_var(new Token("*"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("×", new LSFunction((void*) &jit_mul, 1)));
	add_var(new Token("×"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("/", new LSFunction((void*) &jit_div, 1)));
	add_var(new Token("/"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("÷", new LSFunction((void*) &jit_div, 1)));
	add_var(new Token("÷"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("**", new LSFunction((void*) &jit_pow, 1)));
	add_var(new Token("**"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("%", new LSFunction((void*) &jit_mod, 1)));
	add_var(new Token("%"), op_type, nullptr, nullptr);

	NullSTD().include(this, program);
	BooleanSTD().include(this, program);
	NumberSTD().include(this, program);
	StringSTD().include(this, program);
	ArraySTD().include(this, program);
	ObjectSTD().include(this, program);
	FunctionSTD().include(this, program);
	ClassSTD().include(this, program);
	SystemSTD().include(this, program);

	for (Module* module : modules) {
		module->include(this, program);
	}

	in_program = true;

//	do {
		variables.clear();
		parameters.clear();
		functions.clear();
//		cout << "--------" << endl << "Analyse" << endl << "--------" << endl;
		reanalyse = false;
		program->body->analyse(this, Type::POINTER);
//	} while (reanalyse);

	program->functions = functions;
}

void SemanticAnalyser::enter_function(Function* f) {
	in_function = true;
	variables.push_back(map<std::string, SemanticVar*> {});
	parameters.push_back(map<std::string, SemanticVar*> {});
	loops.push(0);
	functions_stack.push(f);
}

void SemanticAnalyser::leave_function() {
	in_function = false;
	variables.pop_back();
	parameters.pop_back();
	functions_stack.pop();
	loops.pop();
}

void SemanticAnalyser::enter_block() {
	in_block = true;
	variables.push_back(map<std::string, SemanticVar*> {});
}

void SemanticAnalyser::leave_block() {
	in_block = false;
	variables.pop_back();
}

Function* SemanticAnalyser::current_function() const {
	if (functions_stack.empty()) {
		return nullptr;
	}
	return functions_stack.top();
}

void SemanticAnalyser::enter_loop() {
	loops.top()++;
}

void SemanticAnalyser::leave_loop() {
	loops.top()--;
}

bool SemanticAnalyser::in_loop() const {
	return loops.top() > 0;
}

SemanticVar* SemanticAnalyser::add_parameter(Token* v, Type type) {

	SemanticVar* arg = new SemanticVar(VarScope::PARAMETER, type, parameters.back().size(), nullptr, nullptr);
	parameters.back().insert(pair<string, SemanticVar*>(v->content, arg));
	return arg;
}

SemanticVar* SemanticAnalyser::get_var(Token* v) {
	try {
		return internal_vars.at(v->content);
	} catch (exception& e) {}
	try {
		if (parameters.size() > 0) {
			return parameters.back().at(v->content);
		}
	} catch (exception& e) {}

	int i = variables.size() - 1;
	while (i >= 0) {
		try {
			return variables[i].at(v->content);
		} catch (exception& e) {}
		i--;
	}
	add_error({SemanticException::Type::UNDEFINED_VARIABLE, v->line, v->content});
	return nullptr;
}

SemanticVar* SemanticAnalyser::get_var_direct(std::string name) {
	try {
		if (variables.size() > 0) {
			return variables.back().at(name);
		}
	} catch (exception& e) {}
	return nullptr;
}

SemanticVar* SemanticAnalyser::add_var(Token* v, Type type, Value* value, VariableDeclaration* vd) {

	// Internal variable, before execution
	if (!in_program) {
		internal_vars.insert(pair<string, SemanticVar*>(
			v->content,
			new SemanticVar(VarScope::INTERNAL, type, 0, value, vd)
		));
		return internal_vars.at(v->content);
	}

	if (variables.back().find(v->content) != variables.back().end()) {
		add_error({SemanticException::Type::VARIABLE_ALREADY_DEFINED, v->line, v->content});
	}
	variables.back().insert(pair<string, SemanticVar*>(
		v->content,
		new SemanticVar(VarScope::LOCAL, type, 0, value, vd)
	));
	return variables.back().at(v->content);
}

void SemanticAnalyser::add_function(Function* l) {
	functions.push_back(l);
}

map<string, SemanticVar*>& SemanticAnalyser::get_local_vars() {
	return variables.back();
}

void SemanticAnalyser::add_error(SemanticException ex) {
	errors.push_back(ex);
}

} // end of namespace ls
