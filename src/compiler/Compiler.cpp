#include "Compiler.hpp"
#include "../vm/VM.hpp"
#include "../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Compiler::Compiler() {}

Compiler::~Compiler() {}

void Compiler::enter_block() {
	variables.push_back(std::map<std::string, CompilerVar> {});
	if (!loops_blocks.empty()) {
		loops_blocks.back()++;
	}
	functions_blocks.back()++;
}

void Compiler::leave_block(jit_function_t F) {
	delete_variables_block(F, 1);
	variables.pop_back();
	if (!loops_blocks.empty()) {
		loops_blocks.back()--;
	}
	functions_blocks.back()--;
}

void Compiler::delete_variables_block(jit_function_t F, int deepness) {

	for (int i = variables.size() - 1; i >= (int) variables.size() - deepness; --i) {

		for (auto it = variables[i].begin(); it != variables[i].end(); ++it) {

//			std::cout << "delete " << var.first  << std::endl;

			if (it->second.reference == true) {
				continue;
			}
			if (it->second.type.must_manage_memory()) {
				VM::delete_ref(F, it->second.value);
			}
			if (it->second.type == Type::GMP_INT_TMP) {
				VM::delete_gmp_int(F, it->second.value);
			}
			if (it->second.type == Type::GMP_INT) {
				VM::delete_gmp_int(F, it->second.value);
			}
		}
	}
}

void Compiler::enter_function(jit_function_t F) {
	variables.push_back(std::map<std::string, CompilerVar> {});
	functions.push(F);
	functions_blocks.push_back(0);
	this->F = F;
}

void Compiler::leave_function() {
	variables.pop_back();
	functions.pop();
	functions_blocks.pop_back();
	this->F = functions.top();
}

int Compiler::get_current_function_blocks() const {
	return functions_blocks.back();
}

/*
 * Operators
 */
Compiler::value Compiler::insn_and(Compiler::value a, Compiler::value b) const {
	return {jit_insn_and(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_or(Compiler::value a, Compiler::value b) const {
	return {jit_insn_or(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_add(Compiler::value a, Compiler::value b) const {
	return {jit_insn_add(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_eq(Compiler::value a, Compiler::value b) const {
	return {jit_insn_eq(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_lt(Compiler::value a, Compiler::value b) const {
	return {jit_insn_lt(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_mul(Compiler::value a, Compiler::value b) const {
	return {jit_insn_mul(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::new_mpz() const {
	return {VM::create_gmp_int(F, 0), Type::GMP_INT};
}

/*
 * Values
 */
Compiler::value Compiler::new_null() const {
	return {LS_CREATE_POINTER(F, LSNull::get()), Type::NULLL};
}

Compiler::value Compiler::new_bool(bool b) const {
	return {LS_CREATE_BOOLEAN(F, b), Type::BOOLEAN};
}

Compiler::value Compiler::new_integer(int i) const {
	return {LS_CREATE_INTEGER(F, i), Type::INTEGER};
}

Compiler::value Compiler::insn_to_pointer(Compiler::value v) const {
	Type new_type = v.t;
	new_type.nature = Nature::POINTER;
	return {VM::value_to_pointer(F, v.v, v.t), new_type};
}

Compiler::value Compiler::insn_to_bool(Compiler::value v) const {
	if (v.t == Type::FUNCTION_P) {
		return new_bool(true);
	}
	return v;
}

Compiler::value Compiler::insn_address_of(Compiler::value v) const {
	return {jit_insn_address_of(F, v.v), Type::POINTER};
}

Compiler::value Compiler::insn_call(Type return_type, std::vector<Compiler::value> args, void* func) const {
	std::vector<jit_value_t> jit_args;
	std::vector<jit_type_t> arg_types;
	for (const auto& arg : args) {
		jit_args.push_back(arg.v);
		arg_types.push_back(VM::get_jit_type(arg.t));
	}
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, VM::get_jit_type(return_type), arg_types.data(), arg_types.size(), 0);
	return {jit_insn_call_native(F, "call", func, sig, jit_args.data(), arg_types.size(), 0), return_type};
}

/*
 * Variables
 */
void Compiler::add_var(const std::string& name, jit_value_t value, const Type& type, bool ref) {
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
	variables.back()[name].type = type;
}

/*
void Compiler::set_var_type(std::string& name, const Type& type) {
	variables.back().at(name).type = type;
}
*/

/*
std::map<std::string, CompilerVar> Compiler::get_vars() {
	return variables.back();
}
*/

void Compiler::enter_loop(jit_label_t* end_label, jit_label_t* cond_label) {
	loops_end_labels.push_back(end_label);
	loops_cond_labels.push_back(cond_label);
	loops_blocks.push_back(0);
}

void Compiler::leave_loop() {
	loops_end_labels.pop_back();
	loops_cond_labels.pop_back();
	loops_blocks.pop_back();
}

jit_label_t* Compiler::get_current_loop_end_label(int deepness) const {
	return loops_end_labels[loops_end_labels.size() - deepness];
}

jit_label_t* Compiler::get_current_loop_cond_label(int deepness) const {
	return loops_cond_labels[loops_cond_labels.size() - deepness];
}

int Compiler::get_current_loop_blocks(int deepness) const {
	int sum = 0;
	for (size_t i = loops_blocks.size() - deepness; i < loops_blocks.size(); ++i) {
		sum += loops_blocks[i];
	}
	return sum;
}

}
