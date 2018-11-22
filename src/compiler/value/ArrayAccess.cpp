#include "ArrayAccess.hpp"
#include "Array.hpp"
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
	if (key != nullptr) {
		key->print(os, indent, debug);
	}
	if (key2 != nullptr) {
		os << ":";
		key2->print(os, indent, debug);
	}
	os << "]";
	if (debug) {
		os << " " << type;
	}
}

Location ArrayAccess::location() const {
	return {array->location().start, close_bracket->location.end};
}

void ArrayAccess::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	// std::cout << "Analyse AA " << this << " : " << req_type << std::endl;

	array->analyse(analyser, Type::UNKNOWN);

	if (array->type.raw_type != RawType::UNKNOWN and !array->type.raw_type->is_placeholder() and !array->type.is_container()) {
		analyser->add_error({SemanticError::Type::VALUE_MUST_BE_A_CONTAINER, location(), array->location(), {array->to_string()}});
		return;
	}

	if (key == nullptr) {
		return;
	}

	key->analyse(analyser, Type::UNKNOWN);
	constant = array->constant && key->constant;

	array_element_type = Type::UNKNOWN;
	if (array->type.raw_type == RawType::ARRAY || array->type.raw_type == RawType::INTERVAL
		|| array->type.raw_type == RawType::MAP) {
		array_element_type = array->type.getElementType();
		type = array_element_type;
	}
	if (array->type.raw_type == RawType::MAP) {
		map_key_type = array->type.getKeyType();
	}

	if (array->type.raw_type == RawType::INTERVAL) {
		key->analyse(analyser, Type::INTEGER);
	}

	// Range array access : array[4:12], check if the values are numbers
	if (key != nullptr and key2 != nullptr) {

		key->analyse(analyser, Type::INTEGER);
		key2->analyse(analyser, Type::INTEGER);

		if (key->type != Type::UNKNOWN and not key->type.isNumber()) {
			std::string k = "<key 1>";
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, location(), key->location(), {k}});
		}
		if (key2->type != Type::UNKNOWN and not key2->type.isNumber()) {
			std::string k = "<key 2>";
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, location(), key2->location(), {k}});
		}
		type = array->type;

	} else if (array->type.raw_type == RawType::ARRAY or array->type.raw_type == RawType::STRING or array->type.raw_type == RawType::INTERVAL) {

		if (key->type.raw_type != RawType::UNKNOWN and not (key->type.isNumber() or key->type.raw_type == RawType::BOOLEAN)) {
			std::string a = array->to_string();
			std::string k = key->to_string();
			std::string kt = key->type.to_string();
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, location(), key->location(), {k, a, kt}});
		}
		key->analyse(analyser, Type::INTEGER);

		if (array->type.raw_type == RawType::STRING) {
			type = Type::STRING;
		}

	} else if (array->type.raw_type == RawType::MAP) {

		key->analyse(analyser, Type::UNKNOWN);

		if (map_key_type == Type::INTEGER) {
			key->analyse(analyser, Type::INTEGER);
		} else if (map_key_type == Type::REAL) {
			key->analyse(analyser, Type::REAL);
		} else {
			key->analyse(analyser, Type::POINTER);
		}

		if (map_key_type != Type::POINTER) {
			if (map_key_type.nature == Nature::VALUE and key->type.nature == Nature::POINTER) {
				std::string a = array->to_string();
				std::string k = key->to_string();
				std::string kt = key->type.to_string();
				analyser->add_error({SemanticError::Type::INVALID_MAP_KEY, location(), key->location(), {k, a, kt}});
			}
		}
	} else {
		key->analyse(analyser, Type::POINTER);
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
	// TODO should be temporary
	// type.temporary = true;
}

bool ArrayAccess::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int) {

	// std::cout << "ArrayAccess::will_take(" << args << ", " << level << ")" << std::endl;

	type.will_take(args);

	if (Array* arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, args, 1);
	}
	if (ArrayAccess* arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, args, 1);
	}

	type = array->type.getElementType();

	return false;
}

bool ArrayAccess::array_access_will_take(SemanticAnalyser* analyser, const std::vector<Type>& arg_types, int level) {

	type.will_take(arg_types);

	if (auto arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, arg_types, level);
	}
	if (auto arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, arg_types, level + 1);
	}

	type = array->type.getElementType();

	return false;
}

bool ArrayAccess::will_store(SemanticAnalyser* analyser, const Type& type) {
	array->elements_will_store(analyser, type, 1);
	this->type = array->type.getElementType();
	this->types = this->type;
	array_element_type = this->type;
	return false;
}

void ArrayAccess::change_type(SemanticAnalyser* analyser, const Type& new_type) {
	array->will_store(analyser, new_type);
	if (Type::more_specific(new_type, type)) {
		this->type = array->type.getElementType();
		this->types = this->type;
		array_element_type = this->type;
	}
}

Compiler::value ArrayAccess::compile(Compiler& c) const {

	c.mark_offset(open_bracket->location.start.line);

	((ArrayAccess*) this)->compiled_array = array->compile(c);
	array->compile_end(c);

	c.inc_ops(2); // Array access : 2 operations

	if (key2 == nullptr) {

		if (array->type.raw_type == RawType::INTERVAL) {

			auto k = key->compile(c);
			key->compile_end(c);

			auto res = c.insn_call(Type::INTEGER, {compiled_array, k}, +[](LSInterval* interval, int k) {
				return interval->atv(k);
			}, "interval_access");

			if (type.nature == Nature::POINTER) {
				return c.insn_to_pointer(res);
			}
			return res;

		} else if (array->type.raw_type == RawType::MAP) {

			auto k = key->compile(c);
			key->compile_end(c);
			k = c.insn_convert(k, map_key_type);

			void* func = nullptr;
			if (map_key_type == Type::INTEGER) {
				if (array_element_type == Type::INTEGER) {
					func = (void*) &LSMap<int, int>::at;
				} else if (array_element_type == Type::REAL) {
					func = (void*) &LSMap<int, double>::at;
				} else {
					func = (void*) &LSMap<int, LSValue*>::at;
				}
			} else if (map_key_type == Type::REAL) {
				if (array_element_type == Type::INTEGER) {
					func = (void*) &LSMap<double, int>::at;
				} else if (array_element_type == Type::REAL) {
					func = (void*) &LSMap<double, double>::at;
				} else {
					func = (void*) &LSMap<double, LSValue*>::at;
				}
			} else {
				if (array_element_type == Type::INTEGER) {
					func = (void*) &LSMap<LSValue*, int>::at;
				} else if (array_element_type == Type::REAL) {
					func = (void*) &LSMap<LSValue*, double>::at;
				} else {
					func = (void*) &LSMap<LSValue*, LSValue*>::at;
				}
			}

			auto res = c.insn_call(array_element_type, {compiled_array, k}, func, "map_access");

			// if (key->type.must_manage_memory()) {
				c.insn_delete_temporary(k);
			// }

			c.inc_ops(2);

			if (array_element_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
				return c.insn_to_pointer(res);
			}
			return res;

		} else if (array->type.raw_type == RawType::STRING or array->type.raw_type == RawType::ARRAY) {

			auto k = key->compile(c);
			key->compile_end(c);

			if (k.t.nature == Nature::POINTER) {
				k = c.insn_call(Type::INTEGER, {compiled_array, k}, (void*) +[](LSValue* array, LSValue* key_pointer) {
					auto n = dynamic_cast<LSNumber*>(key_pointer);
					if (!n) {
						LSValue::delete_temporary(array);
						LSValue::delete_temporary(key_pointer);
						throw vm::ExceptionObj(vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
					}
					int key_int = n->value;
					LSValue::delete_temporary(key_pointer);
					return key_int;
				}, "at", true);
			}
			k = c.to_int(k);

			// Check index : k < 0 or k >= size
			auto array_size = c.insn_array_size(compiled_array);
			c.insn_if(c.insn_or(c.insn_lt(k, c.new_integer(0)), c.insn_ge(k, array_size)), [&]() {
				c.insn_delete_temporary(compiled_array);
				c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
			});

			if (array->type.raw_type == RawType::STRING) {
				auto e = c.insn_call(Type::STRING, {compiled_array, k}, (void*) &LSString::codePointAt);
				return e;
			} else {
				auto element_addr = c.insn_array_at(compiled_array, k);
				auto e = c.insn_load(element_addr);
				e = c.clone(e);
				if (array_element_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
					return c.insn_to_pointer(e);
				}
				return e;
			}
		} else {
			// Unknown type, call generic at() operator
			auto k = key->compile(c);
			key->compile_end(c);
			auto e = c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSValue* array, LSValue* key) {
				return array->at(key);
			}, "at", true);
			c.insn_delete_temporary(k);
			return e;
		}
	} else {
		auto start = key->compile(c);
		auto end = key2->compile(c);
		key->compile_end(c);
		key2->compile_end(c);
		return c.insn_call(Type::POINTER, {compiled_array, start, end}, (void*) +[](LSValue* a, int start, int end) {
			return a->range(start, end);
		});
	}
}

Compiler::value ArrayAccess::compile_l(Compiler& c) const {

	// Compile the array
	((ArrayAccess*) this)->compiled_array = [&]() {
		if (auto la = dynamic_cast<LeftValue*>(array)) {
			return c.insn_load(la->compile_l(c));
		} else {
			return array->compile(c);
		}
	}();
	array->compile_end(c);

	if (key2 == nullptr) {
		// Compile the key
		auto k = key->compile(c);
		key->compile_end(c);
		// Access
		c.mark_offset(location().start.line);
		if (array->type.raw_type == RawType::ARRAY) {

			// return c.insn_call(Type::POINTER, {compiled_array, k}, +[](LSArray<LSValue*>* array, int key) {
			// 	return array->atL(LSNumber::get(key));
			// });

			auto array_size = c.insn_array_size(compiled_array);
			c.insn_if(c.insn_or(c.insn_lt(k, c.new_integer(0)), c.insn_ge(k, array_size)), [&]() {
				c.insn_delete_temporary(compiled_array);
				c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
			});
			return c.insn_array_at(compiled_array, k);

		} else if (array->type.raw_type == RawType::MAP) {

			if (array->type.not_temporary() == Type::PTR_INT_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<LSValue*, int>* map, LSValue* key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::PTR_REAL_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<LSValue*, double>* map, LSValue* key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::REAL_PTR_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<double, LSValue*>* map, double key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::REAL_INT_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<double, int>* map, double key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::REAL_REAL_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<double, double>* map, double key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::INT_PTR_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<int, LSValue*>* map, int key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::INT_INT_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<int, int>* map, int key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::INT_REAL_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<int, double>* map, int key) {
					return map->atL_base(key);
				});
			} else {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<LSValue*, LSValue*>* map, LSValue* key) {
					return map->atL_base(key);
				});
			}
		} else {
			if (k.t == Type::INTEGER) {
				k = c.insn_to_pointer(k);
			}
			return c.insn_call(type, {compiled_array, k}, (void*) +[](LSValue* array, LSValue* key) {
				return array->atL(key);
			});
		}
	} else {
		auto start = key->compile(c);
		auto end = key2->compile(c);
		key->compile_end(c);
		key2->compile_end(c);
		c.mark_offset(open_bracket->location.start.line);
		return c.insn_call(Type::POINTER, {compiled_array, start, end}, (void*) +[](LSValue* a, int start, int end) {
			// TODO
			a->rangeL(start, end);
		});
	}
}

void ArrayAccess::compile_end(Compiler& c) const {
	c.insn_delete_temporary(compiled_array);
}

Value* ArrayAccess::clone() const {
	auto aa = new ArrayAccess();
	aa->array = array->clone();
	aa->key = key->clone();
	aa->key2 = key2 ? key2->clone() : nullptr;
	aa->open_bracket = open_bracket;
	aa->close_bracket = close_bracket;
	return aa;
}

}
