#include "Compiler.hpp"
#include "../vm/VM.hpp"

using namespace std;

namespace ls {

Compiler::Compiler() {}

Compiler::~Compiler() {}

void Compiler::enter_block() {
	variables.push_back(std::map<std::string, CompilerVar> {});
}

void Compiler::leave_block(jit_function_t F) {

	if (variables.size() > 0) {
		map<string, CompilerVar>& vars = variables.back();

		for (auto it = vars.begin(); it != vars.end(); ++it) {

//			std::cout << "delete " << var.first  << std::endl;

			if (it->second.reference == true) {
				continue;
			}

			if (it->second.type.must_manage_memory()) {
				VM::delete_obj(F, it->second.value);
			}
		}
	}

	variables.pop_back();
}

void Compiler::enter_function(jit_function_t F) {
	variables.push_back(std::map<std::string, CompilerVar> {});
	functions.push(F);
	this->F = F;
}

void Compiler::leave_function() {
	variables.pop_back();
	functions.pop();
	this->F = functions.top();
}

void Compiler::add_var(std::string& name, jit_value_t value, const Type& type, bool ref) {
	variables.back()[name] = {value, type, ref};
}

CompilerVar& Compiler::get_var(const std::string& name) {
	for (int i = variables.size() - 1; i >= 0; --i) {
		auto it = variables[i].find(name);
		if (it != variables[i].end()) {
			return it->second;
		}
	}
	return *((CompilerVar*) nullptr); // Should not reach this line
}

void Compiler::set_var_type(std::string& name, const Type& type) {
	variables.back().at(name).type = type;
}

std::map<std::string, CompilerVar> Compiler::get_vars() {
	return variables.back();
}

void Compiler::enter_loop(jit_label_t* end_label, jit_label_t* cond_label) {
	loops_end_labels.push_back(end_label);
	loops_cond_labels.push_back(cond_label);
}

void Compiler::leave_loop() {
	loops_end_labels.pop_back();
	loops_cond_labels.pop_back();
}

jit_label_t* Compiler::get_current_loop_end_label() const {
	return loops_end_labels.back();
}

jit_label_t* Compiler::get_current_loop_cond_label() const {
	return loops_cond_labels.back();
}

}
