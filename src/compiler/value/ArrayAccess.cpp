#include "../../compiler/value/ArrayAccess.hpp"

#include "../../compiler/value/Array.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSInterval.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"

using namespace std;

namespace ls {

ArrayAccess::ArrayAccess() {
	array = nullptr;
	key = nullptr;
	key2 = nullptr;
	type = Type::POINTER;
}

ArrayAccess::~ArrayAccess() {
	delete array;
	delete key;
	if (key2 != nullptr) {
		delete key2;
	}
}

void ArrayAccess::print(std::ostream& os, int indent, bool debug) const {
	array->print(os, indent, debug);
	os << "[";
	key->print(os, indent, debug);
	if (key2 != nullptr) {
		os << ":";
		key2->print(os, indent, debug);
	}
	os << "]";
	if (debug) {
		os << " " << type;
	}
}

unsigned ArrayAccess::line() const {
	return 0;
}

void ArrayAccess::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	array->analyse(analyser, Type::UNKNOWN);
	key->analyse(analyser, Type::UNKNOWN);
	constant = array->constant && key->constant;

	if (array->type.raw_type == RawType::ARRAY || array->type.raw_type == RawType::INTERVAL
		|| array->type.raw_type == RawType::MAP) {
		array_element_type = array->type.getElementType();
		type = array_element_type;
	}
	if (array->type.raw_type == RawType::MAP) {
		map_key_type = array->type.getKeyType();
	}

	if (array->type == Type::INTERVAL) {
		key->analyse(analyser, Type::INTEGER);
	}

	// Range array access : array[4:12], check if the values are numbers
	if (key != nullptr and key2 != nullptr) {

		key->analyse(analyser, Type::INTEGER);
		key2->analyse(analyser, Type::INTEGER);

		if (key->type != Type::UNKNOWN and not key->type.isNumber()) {
			std::string k = "<key 1>";
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, 0, k});
		}
		if (key2->type != Type::UNKNOWN and not key2->type.isNumber()) {
			std::string k = "<key 2>";
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, 0, k});
		}

		type = array->type;

	} else if (array->type.raw_type == RawType::ARRAY or array->type.raw_type == RawType::STRING
		or array->type.raw_type == RawType::INTERVAL) {

		if (key->type != Type::UNKNOWN and not key->type.isNumber()) {
			std::string k = "<key 1>";
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, 0, k});
		}

		if (array_element_type == Type::INTEGER) {
			key->analyse(analyser, Type::INTEGER);
		} else {
			key->analyse(analyser, Type::POINTER);
		}
	} else if (array->type.raw_type == RawType::MAP) {

		if (map_key_type == Type::INTEGER) {
			key->analyse(analyser, Type::INTEGER);
		} else if (map_key_type == Type::FLOAT) {
			key->analyse(analyser, Type::FLOAT);
		} else {
			key->analyse(analyser, Type::POINTER);
		}
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}

//	cout << "array access " << type << endl;
}

bool ArrayAccess::will_take(SemanticAnalyser* analyser, const std::vector<Type>& arg_types) {

//	cout << "ArrayAccess::will_take " << arg_type << " at " << pos << endl;

	type.will_take(arg_types);

	if (Array* arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, arg_types, 1);
	}
	if (ArrayAccess* arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, arg_types, 1);
	}

	type = array->type.getElementType();

	return false;
}

bool ArrayAccess::array_access_will_take(SemanticAnalyser* analyser, const std::vector<Type>& arg_types, int level) {

	type.will_take(arg_types);

	if (Array* arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, arg_types, level);
	}
	if (ArrayAccess* arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, arg_types, level + 1);
	}

	type = array->type.getElementType();

	return false;
}

void ArrayAccess::change_type(SemanticAnalyser*, const Type&) {
	// TODO
}

/*
 * Array
 */
LSValue* access_temp(LSValue* array, LSValue* key) {
	return array->at(key);
}
int access_temp_value(LSArray<int>* array, int key) {
	return array->atv(key);
}
LSValue** access_l(LSValue* array, LSValue* key) {
	return array->atL(key);
}
int* access_l_value(LSArray<int>* array, int key) {
	return array->atLv(key);
}

int* access_l_map(LSMap<LSValue*, int>* map, LSValue* key) {
	int* res = map->atLv(key);
	LSValue::delete_temporary(key);
	return res;
}

LSValue* range(LSValue* array, int start, int end) {
	LSValue* r = array->range(start, end);
	LSValue::delete_temporary(array);
	return r;
}

int interval_access(const LSInterval* interval, int pos) {
	return interval->atv(pos);
}

jit_value_t ArrayAccess::compile(Compiler& c) const {

//	cout << "aa " << type << endl;

	jit_value_t a = array->compile(c);

	if (key2 == nullptr) {

		if (array->type == Type::INTERVAL) {

			jit_type_t args_types[2] = {LS_POINTER, LS_INTEGER};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 2, 0);

			jit_value_t k = key->compile(c);

			jit_value_t args[] = {a, k};
			jit_value_t res = jit_insn_call_native(c.F, "access", (void*) interval_access, sig, args, 2, JIT_CALL_NOTHROW);

			VM::delete_temporary(c.F, a);

			// Array access : 2 operations
			VM::inc_ops(c.F, 2);

			if (type.nature == Nature::POINTER) {
				return VM::value_to_pointer(c.F, res, type);
			}
			return res;

		} else if (array->type.raw_type == RawType::MAP) {

			jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);

			jit_value_t k = key->compile(c);

			void* func = nullptr;
			if (map_key_type == Type::INTEGER) {
				if (array_element_type == Type::INTEGER) {
					//std::cout << "int int" << std::endl;
					func = (void*) &LSMap<int, int>::at_int_int;
				} else if (array_element_type == Type::FLOAT) {
					//std::cout << "int real" << std::endl;
					func = (void*) &LSMap<int, double>::at_int_real;
				} else {
					//std::cout << "int ptr" << std::endl;
					func = (void*) &LSMap<int, LSValue*>::at_int_ptr;
				}
			} else if (map_key_type == Type::FLOAT) {
				if (array_element_type == Type::INTEGER) {
					//std::cout << "real int" << std::endl;
					func = (void*) &LSMap<double, int>::at_real_int;
				} else if (array_element_type == Type::FLOAT) {
					//std::cout << "real real" << std::endl;
					func = (void*) &LSMap<double, double>::at_real_real;
				} else {
					//std::cout << "real ptr" << std::endl;
					func = (void*) &LSMap<double, LSValue*>::at_real_ptr;
				}
			} else {
				if (array_element_type == Type::INTEGER) {
					//std::cout << "ptr int" << std::endl;
					func = (void*) &LSMap<LSValue*, int>::at_ptr_int;
				} else if (array_element_type == Type::FLOAT) {
					//std::cout << "ptr float" << std::endl;
					func = (void*) &LSMap<LSValue*, double>::at_ptr_real;
				} else {
					//std::cout << "ptr ptr" << std::endl;
					func = (void*) &LSMap<LSValue*, LSValue*>::at;
				}
			}

			jit_value_t args[] = {a, k};
			jit_value_t res = jit_insn_call_native(c.F, "access", func, sig, args, 2, JIT_CALL_NOTHROW);

			if (key->type.must_manage_memory()) {
				VM::delete_temporary(c.F, k);
			}
			VM::delete_temporary(c.F, a);

			VM::inc_ops(c.F, 2);

			if (array_element_type == Type::INTEGER and type.nature == Nature::POINTER) {
				return VM::value_to_pointer(c.F, res, type);
			}
			return res;

		} else {

			jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);

			jit_value_t k = key->compile(c);

			void* func = array_element_type == Type::INTEGER ? (void*) access_temp_value : (void*) access_temp;

			jit_value_t args[] = {a, k};
			jit_value_t res = jit_insn_call_native(c.F, "access", func, sig, args, 2, JIT_CALL_NOTHROW);

			if (key->type.must_manage_memory()) {
				VM::delete_temporary(c.F, k);
			}
			VM::delete_temporary(c.F, a);

			// Array access : 2 operations
			VM::inc_ops(c.F, 2);

			if (array_element_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
				return VM::value_to_pointer(c.F, res, type);
			}
			return res;
		}

	} else {

		jit_type_t args_types[3] = {LS_POINTER, LS_INTEGER, LS_INTEGER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 3, 0);

		jit_value_t start = key->compile(c);
		jit_value_t end = key2->compile(c);
		jit_value_t args[] = {a, start, end};

		jit_value_t result = jit_insn_call_native(c.F, "range", (void*) range, sig, args, 3, JIT_CALL_NOTHROW);

		return result;
	}
}

jit_value_t ArrayAccess::compile_l(Compiler& c) const {

	//std::cout << "compile access l " << std::endl;

	jit_value_t a = array->compile(c);

	jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);

	jit_value_t k = key->compile(c);

	jit_value_t args[] = {a, k};

	if (array->type.raw_type == RawType::MAP) {
		return jit_insn_call_native(c.F, "access_l", (void*) access_l_map, sig, args, 2, JIT_CALL_NOTHROW);
	} else {
		return jit_insn_call_native(c.F, "access_l", (void*) access_l_value, sig, args, 2, JIT_CALL_NOTHROW);
	}
}
}
