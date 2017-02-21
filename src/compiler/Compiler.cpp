#include "Compiler.hpp"
#include "../vm/VM.hpp"
#include "../vm/value/LSNull.hpp"
#include "../vm/value/LSArray.hpp"
#include "../vm/value/LSMap.hpp"
#include "../vm/Program.hpp"
#include "../../lib/utf8.h"

using namespace std;

namespace ls {

Compiler::Compiler(Program* program) : program(program) {}

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
			if (it->second.type.must_manage_memory()) {
				insn_delete({it->second.value, Type::POINTER});
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

void Compiler::delete_function_variables() {
	for (const auto& v : function_variables.back()) {
		if (v.type.must_manage_memory()) {
			insn_delete({v.value, Type::POINTER});
		}
		if (v.type == Type::GMP_INT_TMP) {
			VM::delete_gmp_int(F, v.value);
		}
		if (v.type == Type::GMP_INT) {
			VM::delete_gmp_int(F, v.value);
		}
	}
}

void Compiler::enter_function(jit_function_t F) {
	variables.push_back(std::map<std::string, CompilerVar> {});
	function_variables.push_back(std::vector<CompilerVar> {});
	functions.push(F);
	functions_blocks.push_back(0);
	this->F = F;
}

void Compiler::leave_function() {
	variables.pop_back();
	function_variables.pop_back();
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
void Compiler::insn_store(Compiler::value a, Compiler::value b) const {
	jit_insn_store(F, a.v, b.v);
}
void Compiler::insn_store_relative(Compiler::value a, int pos, Compiler::value b) const {
	jit_insn_store_relative(F, a.v, pos, b.v);
}
Compiler::value Compiler::insn_not(Compiler::value v) const {
	return {jit_insn_not(F, v.v), v.t};
}
Compiler::value Compiler::insn_and(Compiler::value a, Compiler::value b) const {
	return {jit_insn_and(F, insn_to_bool(a).v, insn_to_bool(b).v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_or(Compiler::value a, Compiler::value b) const {
	return {jit_insn_or(F, insn_to_bool(a).v, insn_to_bool(b).v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_add(Compiler::value a, Compiler::value b) const {
	auto result_type = [&]() {
		if (a.t == Type::REAL or b.t == Type::REAL) return Type::REAL;
		if (a.t == Type::LONG or b.t == Type::LONG) return Type::LONG;
		return Type::INTEGER;
	}();
	return {jit_insn_add(F, a.v, b.v), result_type};
}
Compiler::value Compiler::insn_sub(Compiler::value a, Compiler::value b) const {
	auto result_type = [&]() {
		if (a.t == Type::REAL or b.t == Type::REAL) return Type::REAL;
		if (a.t == Type::LONG or b.t == Type::LONG) return Type::LONG;
		return Type::INTEGER;
	}();
	return {jit_insn_sub(F, a.v, b.v), result_type};
}
Compiler::value Compiler::insn_eq(Compiler::value a, Compiler::value b) const {
	return {jit_insn_eq(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_ne(Compiler::value a, Compiler::value b) const {
	return {jit_insn_ne(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_lt(Compiler::value a, Compiler::value b) const {
	return {jit_insn_lt(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_le(Compiler::value a, Compiler::value b) const {
	return {jit_insn_le(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_gt(Compiler::value a, Compiler::value b) const {
	return {jit_insn_gt(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_ge(Compiler::value a, Compiler::value b) const {
	return {jit_insn_ge(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_mul(Compiler::value a, Compiler::value b) const {
	return {jit_insn_mul(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_div(Compiler::value a, Compiler::value b) const {
	return {jit_insn_div(F, jit_insn_convert(F, a.v, LS_REAL, 0), b.v), Type::REAL};
}
Compiler::value Compiler::insn_int_div(Compiler::value a, Compiler::value b) const {
	return {jit_insn_div(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_bit_and(Compiler::value a, Compiler::value b) const {
	return {jit_insn_and(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_bit_or(Compiler::value a, Compiler::value b) const {
	return {jit_insn_or(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_bit_xor(Compiler::value a, Compiler::value b) const {
	return {jit_insn_xor(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_mod(Compiler::value a, Compiler::value b) const {
	return {jit_insn_rem(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_pow(Compiler::value a, Compiler::value b) const {
	return {jit_insn_pow(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_log10(Compiler::value a) const {
	return {jit_insn_log10(F, a.v), Type::INTEGER};
}

/*
 * Values
 */
Compiler::value Compiler::clone(Compiler::value v) const {
	if (v.t.nature == Nature::POINTER) {
		return insn_call(v.t, {v}, (void*)+[](LSValue* value) {
			return value->clone();
		});
	}
	return v;
}
Compiler::value Compiler::new_null() const {
	return {LS_CREATE_POINTER(F, LSNull::get()), Type::NULLL};
}
Compiler::value Compiler::new_bool(bool b) const {
	return {LS_CREATE_BOOLEAN(F, b), Type::BOOLEAN};
}
Compiler::value Compiler::new_integer(int i) const {
	return {LS_CREATE_INTEGER(F, i), Type::INTEGER};
}
Compiler::value Compiler::new_pointer(const void* p) const {
	return {LS_CREATE_POINTER(F, (void*) p), Type::POINTER};
}
Compiler::value Compiler::new_mpz() const {
	return {VM::create_gmp_int(F, 0), Type::GMP_INT_TMP};
}
Compiler::value Compiler::new_object(Compiler::value clazz) const {
	return insn_call(Type::POINTER, {clazz}, +[](LSClass* clazz) {
		return new LSObject(clazz);
	});
}

Compiler::value Compiler::to_int(Compiler::value v) const {
	return {jit_insn_convert(F, v.v, LS_INTEGER, 0), Type::INTEGER};
}

Compiler::value Compiler::insn_to_pointer(Compiler::value v) const {
	if (v.t.nature == Nature::POINTER) {
		return v; // already a pointer
	}
	Type new_type = v.t;
	new_type.nature = Nature::POINTER;
	return {VM::value_to_pointer(F, v.v, v.t), new_type};
}

Compiler::value Compiler::insn_to_bool(Compiler::value v) const {
	if (v.t == Type::BOOLEAN) {
		return v;
	}
	if (v.t == Type::INTEGER) {
		return {jit_insn_to_bool(F, v.v), Type::BOOLEAN};
	}
	if (v.t.raw_type == RawType::STRING) {
		return insn_call(Type::BOOLEAN, {v}, (void*) &LSString::isTrue);
	}
	if (v.t.raw_type == RawType::ARRAY) {
		// Always take LSArray<int>, but the array is not necessarily of this type
		return insn_call(Type::BOOLEAN, {v}, (void*) &LSArray<int>::isTrue);
	}
	if (v.t.raw_type == RawType::FUNCTION) {
		return new_bool(true);
	}
	// TODO other types
	return v;
}

Compiler::value Compiler::insn_address_of(Compiler::value v) const {
	return {jit_insn_address_of(F, v.v), Type::POINTER};
}

Compiler::value Compiler::insn_load(Compiler::value v, int pos, Type t) const {
	return {jit_insn_load_relative(F, v.v, pos, VM::get_jit_type(t)), t};
}

Compiler::value Compiler::insn_typeof(Compiler::value v) const {
	if (v.t == Type::NULLL) return new_integer(LSValue::NULLL);
	if (v.t == Type::BOOLEAN) return new_integer(LSValue::BOOLEAN);
	if (v.t.isNumber()) return new_integer(LSValue::NUMBER);
	if (v.t == Type::STRING) return new_integer(LSValue::STRING);
	if (v.t == Type::ARRAY) return new_integer(LSValue::ARRAY);
	if (v.t == Type::MAP) return new_integer(LSValue::MAP);
	if (v.t == Type::SET) return new_integer(LSValue::SET);
	if (v.t == Type::INTERVAL) return new_integer(LSValue::INTERVAL);
	if (v.t == Type::FUNCTION) return new_integer(LSValue::FUNCTION);
	if (v.t == Type::OBJECT) return new_integer(LSValue::OBJECT);
	if (v.t == Type::CLASS) return new_integer(LSValue::CLASS);
	return insn_call(Type::INTEGER, {v}, +[](LSValue* v) {
		return v->type;
	});
}

Compiler::value Compiler::insn_class_of(Compiler::value v) const {
	if (v.t == Type::NULLL)
		return new_pointer(vm->system_vars["Null"]);
	if (v.t == Type::BOOLEAN)
		return new_pointer(vm->system_vars["Boolean"]);
	if (v.t.isNumber())
		return new_pointer(vm->system_vars["Number"]);
	if (v.t == Type::STRING)
		return new_pointer(vm->system_vars["String"]);
	if (v.t.raw_type == RawType::ARRAY)
		return new_pointer(vm->system_vars["Array"]);
	if (v.t.raw_type == RawType::MAP)
		return new_pointer(vm->system_vars["Map"]);
	if (v.t.raw_type == RawType::SET)
		return new_pointer(vm->system_vars["Set"]);
	if (v.t.raw_type == RawType::INTERVAL)
		return new_pointer(vm->system_vars["Interval"]);
	if (v.t.raw_type == RawType::FUNCTION)
		return new_pointer(vm->system_vars["Function"]);
	if (v.t == Type::OBJECT)
		return new_pointer(vm->system_vars["Object"]);
	if (v.t == Type::CLASS)
		return new_pointer(vm->system_vars["Class"]);

	return insn_call(Type::CLASS, {v}, +[](LSValue* v) {
		return v->getClass();
	});
}

void Compiler::insn_delete(Compiler::value v) const {
	if (v.t.nature == Nature::POINTER) {
		insn_call(Type::VOID, {v}, (void*) &LSValue::delete_ref);
	}
}

void Compiler::insn_delete_temporary(Compiler::value v) const {
	if (v.t.nature == Nature::POINTER) {
		insn_call(Type::VOID, {v}, (void*) &LSValue::delete_temporary);
	}
}

Compiler::value Compiler::insn_array_size(Compiler::value v) const {
	if (v.t == Type::INT_ARRAY) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<int>::int_size);
	} else if (v.t == Type::REAL_ARRAY) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<double>::int_size);
	} else {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<LSValue*>::int_size);
	}
}

Compiler::value Compiler::insn_get_capture(int index, Type type) const {
	jit_value_t fun = jit_value_get_param(F, 0); // function pointer
	jit_value_t v =	VM::function_get_capture(F, fun, index);
	if (type.nature == Nature::VALUE) {
		v = VM::pointer_to_value(F, v, type);
	}
	return {v, type};
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
 * Iterators
 */
Compiler::value Compiler::iterator_begin(Compiler::value v) const {
	if (v.t.raw_type == RawType::ARRAY) {
		Compiler::value it = {jit_value_create(F, VM::get_jit_type(v.t)), v.t};
		insn_store(it, insn_load(v, 24));
		return it;
	}
	if (v.t.raw_type == RawType::INTERVAL) {
		jit_type_t types[2] = {jit_type_void_ptr, jit_type_int};
		auto interval_iterator = jit_type_create_struct(types, 2, 0);
		Compiler::value it = {jit_value_create(F, interval_iterator), Type::INTERVAL_ITERATOR};
		auto addr = insn_address_of(it);
		jit_insn_store_relative(F, addr.v, 0, v.v);
		jit_insn_store_relative(F, addr.v, 8, insn_load(v, 20, Type::INTEGER).v);
		return it;
	}
	if (v.t.raw_type == RawType::STRING) {
		jit_type_t types[5] = {jit_type_void_ptr, jit_type_int, jit_type_int, jit_type_int, jit_type_int};
		auto string_iterator = jit_type_create_struct(types, 5, 0);
		Compiler::value it = {jit_value_create(F, string_iterator), Type::STRING_ITERATOR};
		auto addr = insn_address_of(it);
		insn_call(Type::VOID, {v, addr}, (void*) +[](LSString* str, LSString::iterator* it) {
			auto i = LSString::iterator_begin(str);
			it->buffer = i.buffer;
			it->index = 0;
			it->pos = 0;
			it->next_pos = 0;
			it->character = 0;
		});
		return it;
	}
	if (v.t.raw_type == RawType::MAP) {
		return insn_load(v, 48, v.t);
	}
	if (v.t == Type::INTEGER) {
		jit_type_t types[3] = {jit_type_int, jit_type_int, jit_type_int};
		auto integer_iterator = jit_type_create_struct(types, 3, 0);
		Compiler::value it = {jit_value_create(F, integer_iterator), Type::LONG};
		auto addr = jit_insn_address_of(F, it.v);
		jit_insn_store_relative(F, addr, 0, v.v);
		jit_insn_store_relative(F, addr, 4, to_int(insn_pow(new_integer(10), to_int(insn_log10(v)))).v);
		jit_insn_store_relative(F, addr, 8, new_integer(0).v);
		return it;
	}
}

Compiler::value Compiler::iterator_end(Compiler::value v, Compiler::value it) const {
	if (v.t.raw_type == RawType::ARRAY) {
		return insn_eq(it, insn_load(v, 32));
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		auto addr = insn_address_of(it);
		auto interval = insn_load(addr, 0, Type::POINTER);
		auto end = insn_load(interval, 24, Type::INTEGER);
		auto pos = insn_load(addr, 8, Type::INTEGER);
		return insn_gt(pos, end);
	}
	if (it.t == Type::STRING_ITERATOR) {
		auto addr = insn_address_of(it);
		return insn_call(Type::BOOLEAN, {addr}, &LSString::iterator_end);
	}
	if (v.t.raw_type == RawType::MAP) {
		auto end = insn_add(v, new_integer(32)); // end_ptr = &map + 24
		return insn_eq(it, end);
	}
	if (v.t == Type::INTEGER) {
		auto addr = insn_address_of(it);
		auto p = insn_load(addr, 4, Type::INTEGER);
		return insn_eq(p, new_integer(0));
	}
}

Compiler::value Compiler::iterator_key(Compiler::value v, Compiler::value it) const {
	if (it.t.raw_type == RawType::ARRAY) {
		return insn_int_div(insn_sub(it, insn_load(v, 24)), new_integer(it.t.element().size() / 8));
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		auto addr = insn_address_of(it);
		auto interval = insn_load(addr, 0);
		auto start = insn_load(interval, 20);
		auto e = insn_load(addr, 8, Type::INTEGER);
		return insn_sub(e, start);
	}
	if (it.t == Type::STRING_ITERATOR) {
		auto addr = insn_address_of(it);
		return insn_call(Type::INTEGER, {addr}, &LSString::iterator_key);
	}
	if (it.t.raw_type == RawType::MAP) {
		auto key = insn_load(it, 32, it.t.getKeyType());
		return key;
	}
	if (it.t == Type::LONG) {
		auto addr = insn_address_of(it);
		return insn_load(addr, 8, Type::INTEGER);
	}
}

Compiler::value Compiler::iterator_get(Compiler::value it) const {
	if (it.t.raw_type == RawType::ARRAY) {
		return insn_load(it, 0, it.t.getElementType());
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		auto addr = insn_address_of(it);
		auto e = insn_load(addr, 8, Type::INTEGER);
		return e;
	}
	if (it.t == Type::STRING_ITERATOR) {
		auto addr = insn_address_of(it);
		auto int_char = insn_call(Type::INTEGER, {addr}, &LSString::iterator_get);
		return insn_call(Type::STRING, {int_char}, (void*) +[](unsigned int c) {
			char dest[5];
			u8_toutf8(dest, 5, &c, 1);
			return new LSString(dest);
		});
	}
	if (it.t.raw_type == RawType::MAP) {
		auto element = insn_load(it, 32 + 8, it.t.element());
		return element;
	}
	if (it.t == Type::LONG) {
		auto addr = insn_address_of(it);
		auto n = insn_load(addr, 0, Type::INTEGER);
		auto p = insn_load(addr, 4, Type::INTEGER);
		return insn_int_div(n, p);
	}
}

void Compiler::iterator_increment(Compiler::value it) const {
	if (it.t.raw_type == RawType::ARRAY) {
		insn_store(it, insn_add(it, new_integer(it.t.element().size() / 8)));
		//insn_store(it, insn_add(it, insn_mul(new_integer(16), new_integer(it.t.element().size() / 8)) ));
		return;
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		auto addr = insn_address_of(it);
		auto pos = insn_load(addr, 8, Type::INTEGER);
		insn_store_relative(addr, 8, insn_add(pos, new_integer(1)));
		return;
	}
	if (it.t == Type::STRING_ITERATOR) {
		auto addr = insn_address_of(it);
		insn_call(Type::VOID, {addr}, &LSString::iterator_next);
		return;
	}
	if (it.t.raw_type == RawType::MAP) {
		insn_store(it, insn_call(Type::POINTER, {it}, (void*) +[](LSMap<int, int>::iterator it) {
			it++;
			return it;
		}));
		return;
	}
	if (it.t == Type::LONG) {
		auto addr = insn_address_of(it);
		auto n = insn_load(addr, 0, Type::INTEGER);
		auto p = insn_load(addr, 4, Type::INTEGER);
		auto i = insn_load(addr, 8, Type::INTEGER);
		jit_insn_store_relative(F, addr.v, 0, insn_mod(n, p).v);
		jit_insn_store_relative(F, addr.v, 4, insn_int_div(p, new_integer(10)).v);
		jit_insn_store_relative(F, addr.v, 8, insn_add(i, new_integer(1)).v);
		return;
	}
}

/*
 * Controls
 */
void Compiler::insn_if(Compiler::value condition, std::function<void()> then) const {
	jit_label_t label_end = jit_label_undefined;
	jit_insn_branch_if_not(F, condition.v, &label_end);
	then();
	jit_insn_label(F, &label_end);
}

void Compiler::insn_throw(Compiler::value v) const {
	jit_insn_throw(F, v.v);
}

/*
 * Variables
 */
void Compiler::add_var(const std::string& name, jit_value_t value, const Type& type, bool ref) {
	variables.back()[name] = {value, type, ref};
}

void Compiler::add_function_var(jit_value_t value, const Type& type) {
	function_variables.back().push_back({value, type, false});
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
