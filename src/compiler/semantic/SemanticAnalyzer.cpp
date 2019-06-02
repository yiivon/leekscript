#include "SemanticAnalyzer.hpp"
#include "../../compiler/instruction/ExpressionInstruction.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Context.hpp"
#include "../error/Error.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/Module.hpp"
#include <functional>

namespace ls {

SemanticAnalyzer::SemanticAnalyzer() {
	program = nullptr;
//	loops.push(0);
//	variables.push_back(vector<map<std::string, SemanticVar*>> {});
//	functions_stack.push(nullptr); // The first function is the main function of the program
//	parameters.push_back(map<std::string, SemanticVar*> {});
}

SemanticAnalyzer::~SemanticAnalyzer() {}

void SemanticVar::must_be_any(SemanticAnalyzer* analyzer) {
	if (value != nullptr) {
		value->must_be_any(analyzer);
	}
}

void SemanticAnalyzer::analyze(Program* program, Context* context) {

	this->program = program;

	enter_function(program->main);

	// Add context variables
	if (context) {
		for (auto var : context->vars) {
			// std::cout << "Add context var " << var.first << std::endl;
			add_var(new Token(TokenType::IDENT, program->main_file, 0, 0, 0, var.first), var.second.type, nullptr, nullptr);
		}
	}
	program->analyze(this);
	program->functions = functions;
}

void SemanticAnalyzer::enter_function(Function* f) {

	// Create function scope
	variables.push_back(std::vector<std::map<std::string, std::shared_ptr<SemanticVar>>> {});
	// First function block
	variables.back().push_back(std::map<std::string, std::shared_ptr<SemanticVar>> {});
	// Parameters
	parameters.push_back(std::map<std::string, std::shared_ptr<SemanticVar>> {});

	loops.push(0);
	functions_stack.push(f);
}

void SemanticAnalyzer::leave_function() {
	variables.pop_back();
	parameters.pop_back();
	functions_stack.pop();
	loops.pop();
}

void SemanticAnalyzer::enter_block() {
	variables.back().push_back(std::map<std::string, std::shared_ptr<SemanticVar>> {});
}

void SemanticAnalyzer::leave_block() {
	variables.back().pop_back();
}

Function* SemanticAnalyzer::current_function() const {
	return functions_stack.top();
}

void SemanticAnalyzer::enter_loop() {
	loops.top()++;
}

void SemanticAnalyzer::leave_loop() {
	loops.top()--;
}

bool SemanticAnalyzer::in_loop(int deepness) const {
	return loops.top() >= deepness;
}

std::shared_ptr<SemanticVar> SemanticAnalyzer::add_parameter(Token* v, const Type* type) {
	auto arg = std::make_shared<SemanticVar>(v->content, VarScope::PARAMETER, type, parameters.back().size(), nullptr, nullptr, current_function(), nullptr);
	parameters.back().insert({v->content, arg});
	return arg;
}

std::shared_ptr<SemanticVar> SemanticAnalyzer::get_var(Token* v) {

	// Search in interval variables : global for the program
	try {
		return vm->internal_vars.at(v->content);
	} catch (std::exception& e) {}

	// Search operators
	if (auto op = program->get_operator(v->content)) {
		return op;
	}

	// Search recursively in the functions
	int f = functions_stack.size() - 1;
	while (f >= 0) {
		// Search in the function parameters
		try {
			return parameters.at(f).at(v->content);
		} catch (std::exception& e) {}

		// Search in the local variables of the function
		int b = variables.at(f).size() - 1;
		while (b >= 0) {
			try {
				return variables.at(f).at(b).at(v->content);
			} catch (std::exception& e) {}
			b--;
		}
		f--;
	}
	return nullptr;
}

std::shared_ptr<SemanticVar> SemanticAnalyzer::add_var(Token* v, const Type* type, Value* value, VariableDeclaration* vd) {
	if (vm->internal_vars.find(v->content) != vm->internal_vars.end()) {
		add_error({Error::Type::VARIABLE_ALREADY_DEFINED, v->location, v->location, {v->content}});
		return nullptr;
	}
	if (variables.back().back().find(v->content) != variables.back().back().end()) {
		add_error({Error::Type::VARIABLE_ALREADY_DEFINED, v->location, v->location, {v->content}});
		return nullptr;
	}
	variables.back().back().insert(std::pair<std::string, std::shared_ptr<SemanticVar>>(
		v->content,
		std::make_shared<SemanticVar>(v->content, VarScope::LOCAL, type, 0, value, vd, current_function(), nullptr)
	));
	return variables.back().back().at(v->content);
}

void SemanticAnalyzer::add_function(Function* l) {
	functions.push_back(l);
}

std::map<std::string, std::shared_ptr<SemanticVar>>& SemanticAnalyzer::get_local_vars() {
	return variables.back().back();
}

std::shared_ptr<SemanticVar> SemanticAnalyzer::convert_var_to_any(std::shared_ptr<SemanticVar> var) {
	// std::cout << "SemanticAnalyser::convert_var_to_any(" << var->name << ")" << std::endl;
	if (var->type->is_polymorphic()) return var;
	auto new_var = std::make_shared<SemanticVar>(var->name, var->scope, Type::any, 0, nullptr, nullptr, var->function, nullptr);
	// Search recursively in the functions

	int f = functions_stack.size() - 1;
	while (f >= 0) {
		// Search in the function parameters
		auto& params = parameters.at(f);
		if (params.find(var->name) != params.end()) {
			params.at(var->name) = new_var;
		}
		// Search in the local variables of the function
		int b = variables.at(f).size() - 1;
		while (b >= 0) {
			auto& vars = variables.at(f).at(b);
			if (vars.find(var->name) != vars.end()) {
				vars.at(var->name) = new_var;
			}
			b--;
		}
		f--;
	}
	return new_var;
}

void SemanticAnalyzer::add_error(Error ex) {
	ex.underline_code = program->underline_code(ex.location, ex.focus);
	errors.push_back(ex);
}

} // end of namespace ls
