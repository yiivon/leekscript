#include "ArrayAccess.hpp"
#include "Array.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSInterval.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"

namespace ls {

ArrayAccess::ArrayAccess() {
	array = nullptr;
	key = nullptr;
	key2 = nullptr;
	type = Type::any();
}

ArrayAccess::~ArrayAccess() {
	delete array;
	delete key;
	if (key2 != nullptr) {
		delete key2;
	}
}

bool ArrayAccess::isLeftValue() const {
	return key2 == nullptr; // Range access is not left-value (yet)
}

void ArrayAccess::print(std::ostream& os, int indent, bool debug, bool condensed) const {
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

void ArrayAccess::analyse(SemanticAnalyser* analyser) {

	// std::cout << "Analyse AA " << this << " : " << req_type << std::endl;

	array->analyse(analyser);

	if (not array->type.can_be_container()) {
		analyser->add_error({SemanticError::Type::VALUE_MUST_BE_A_CONTAINER, location(), array->location(), {array->to_string()}});
		return;
	}
	if (key == nullptr) {
		return;
	}

	key->analyse(analyser);
	constant = array->constant && key->constant;

	array_element_type = {};
	if (array->type.is_array() || array->type.is_interval() || array->type.is_map()) {
		array_element_type = array->type.element();
		type = array_element_type;
	} else {
		type = array->type.element();
	}
	if (array->type.is_map()) {
		map_key_type = array->type.key();
	}

	// Range array access : array[4:12], check if the values are numbers
	if (key != nullptr and key2 != nullptr) {

		key2->analyse(analyser);

		if (!key->type.is_any() and not key->type.is_number()) {
			std::string k = "<key 1>";
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, location(), key->location(), {k}});
		}
		if (!key2->type.is_any() and not key2->type.is_number()) {
			std::string k = "<key 2>";
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, location(), key2->location(), {k}});
		}
		type = array->type;

	} else if (array->type.is_array() or array->type.is_string() or array->type.is_interval()) {
		if (not key->type.can_be_numeric()) {
			std::string a = array->to_string();
			std::string k = key->to_string();
			std::string kt = key->type.to_string();
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, location(), key->location(), {k, a, kt}});
		}
		if (array->type.is_string()) {
			type = Type::string();
		}
	} else if (array->type.is_map()) {
		if (!key->type.castable(map_key_type)) {
			std::string a = array->to_string();
			std::string k = key->to_string();
			std::string kt = key->type.to_string();
			analyser->add_error({SemanticError::Type::INVALID_MAP_KEY, location(), key->location(), {k, a, kt}});
		}
	}
	// TODO should be temporary
	// type.temporary = true;
}

bool ArrayAccess::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int) {

	// std::cout << "ArrayAccess::will_take(" << args << ", " << level << ")" << std::endl;

	if (Array* arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, args, 1);
	}
	if (ArrayAccess* arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, args, 1);
	}

	type = array->type.element().fold();

	return false;
}

bool ArrayAccess::array_access_will_take(SemanticAnalyser* analyser, const std::vector<Type>& arg_types, int level) {

	if (auto arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, arg_types, level);
	}
	if (auto arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, arg_types, level + 1);
	}

	type = array->type.element().fold();

	return false;
}

bool ArrayAccess::will_store(SemanticAnalyser* analyser, const Type& type) {
	array->elements_will_store(analyser, type, 1);
	this->type = array->type.element().fold();
	array_element_type = this->type;
	return false;
}

void ArrayAccess::change_value(SemanticAnalyser* analyser, Value* value) {
	array->will_store(analyser, value->type);
	if (!type.is_any()) {
		this->type = array->type.element().fold();
		array_element_type = this->type;
	}
}

Compiler::value ArrayAccess::compile(Compiler& c) const {

	c.mark_offset(open_bracket->location.start.line);

	((ArrayAccess*) this)->compiled_array = array->compile(c);
	array->compile_end(c);

	c.inc_ops(2); // Array access : 2 operations

	if (key2 == nullptr) {

		if (array->type.is_interval()) {

			auto k = key->compile(c);
			key->compile_end(c);
			return c.insn_invoke(Type::integer(), {compiled_array, k}, +[](LSInterval* interval, int k) {
				return interval->atv(k);
			});

		} else if (array->type.is_map()) {

			auto k = key->compile(c);
			key->compile_end(c);
			k = c.insn_convert(k, map_key_type);

			void* func = nullptr;
			if (map_key_type == Type::integer()) {
				if (array_element_type == Type::integer()) {
					func = (void*) &LSMap<int, int>::at;
				} else if (array_element_type == Type::real()) {
					func = (void*) &LSMap<int, double>::at;
				} else {
					func = (void*) &LSMap<int, LSValue*>::at;
				}
			} else if (map_key_type == Type::real()) {
				if (array_element_type == Type::integer()) {
					func = (void*) &LSMap<double, int>::at;
				} else if (array_element_type == Type::real()) {
					func = (void*) &LSMap<double, double>::at;
				} else {
					func = (void*) &LSMap<double, LSValue*>::at;
				}
			} else {
				if (array_element_type == Type::integer()) {
					func = (void*) &LSMap<LSValue*, int>::at;
				} else if (array_element_type == Type::real()) {
					func = (void*) &LSMap<LSValue*, double>::at;
				} else {
					func = (void*) &LSMap<LSValue*, LSValue*>::at;
				}
			}
			auto res = c.insn_invoke(array_element_type, {compiled_array, k}, func);
			c.insn_delete_temporary(k);
			c.inc_ops(2);
			return res;

		} else if (array->type.is_string() or array->type.is_array()) {

			auto k = key->compile(c);
			key->compile_end(c);

			if (k.t.is_polymorphic()) {
				k = c.insn_invoke(Type::integer(), {compiled_array, k}, (void*) +[](LSValue* array, LSValue* key_pointer) {
					auto n = dynamic_cast<LSNumber*>(key_pointer);
					if (!n) {
						LSValue::delete_temporary(array);
						LSValue::delete_temporary(key_pointer);
						throw vm::ExceptionObj(vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
					}
					int key_int = n->value;
					LSValue::delete_temporary(key_pointer);
					return key_int;
				});
			}
			k = c.to_int(k);

			// Check index : k < 0 or k >= size
			auto array_size = c.insn_array_size(compiled_array);
			c.insn_if(c.insn_or(c.insn_lt(k, c.new_integer(0)), c.insn_ge(k, array_size)), [&]() {
				c.insn_delete_temporary(compiled_array);
				c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
			});

			if (array->type.is_string()) {
				auto e = c.insn_call(Type::string(), {compiled_array, k}, (void*) &LSString::codePointAt);
				return e;
			} else {
				auto element_addr = c.insn_array_at(compiled_array, k);
				auto e = c.insn_load(element_addr);
				return c.clone(e);
			}
		} else {
			// Unknown type, call generic at() operator
			auto k = c.insn_to_any(key->compile(c));
			key->compile_end(c);
			auto e = c.insn_invoke(Type::any(), {compiled_array, k}, (void*) +[](LSValue* array, LSValue* key) {
				return array->at(key);
			});
			c.insn_delete_temporary(k);
			return e;
		}
	} else {
		auto start = key->compile(c);
		auto end = key2->compile(c);
		key->compile_end(c);
		key2->compile_end(c);
		return c.insn_invoke(Type::any(), {compiled_array, start, end}, (void*) +[](LSValue* a, int start, int end) {
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
		if (array->type.is_array()) {

			// return c.insn_call(Type::POINTER, {compiled_array, k}, +[](LSArray<LSValue*>* array, int key) {
			// 	return array->atL(LSNumber::get(key));
			// });

			auto array_size = c.insn_array_size(compiled_array);
			c.insn_if(c.insn_or(c.insn_lt(k, c.new_integer(0)), c.insn_ge(k, array_size)), [&]() {
				c.insn_delete_temporary(compiled_array);
				c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
			});
			return c.insn_array_at(compiled_array, k);

		} else if (array->type.is_map()) {

			if (array->type.not_temporary() == Type::map(Type::any(), Type::integer())) {
				return c.insn_call(Type::integer().pointer(), {compiled_array, k}, (void*) +[](LSMap<LSValue*, int>* map, LSValue* key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::map(Type::any(), Type::real())) {
				return c.insn_call(Type::real().pointer(), {compiled_array, k}, (void*) +[](LSMap<LSValue*, double>* map, LSValue* key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::map(Type::real(), Type::any())) {
				return c.insn_call(Type::any().pointer(), {compiled_array, k}, (void*) +[](LSMap<double, LSValue*>* map, double key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::map(Type::real(), Type::integer())) {
				return c.insn_call(Type::integer().pointer(), {compiled_array, k}, (void*) +[](LSMap<double, int>* map, double key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::map(Type::real(), Type::real())) {
				return c.insn_call(Type::real().pointer(), {compiled_array, k}, (void*) +[](LSMap<double, double>* map, double key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::map(Type::integer(), Type::any())) {
				return c.insn_call(Type::any().pointer(), {compiled_array, k}, (void*) +[](LSMap<int, LSValue*>* map, int key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::map(Type::integer(), Type::integer())) {
				return c.insn_call(Type::integer().pointer(), {compiled_array, k}, (void*) +[](LSMap<int, int>* map, int key) {
					return map->atL_base(key);
				});
			} else if (array->type.not_temporary() == Type::map(Type::integer(), Type::real())) {
				return c.insn_call(Type::real().pointer(), {compiled_array, k}, (void*) +[](LSMap<int, double>* map, int key) {
					return map->atL_base(key);
				});
			} else {
				return c.insn_call(Type::any().pointer(), {compiled_array, k}, (void*) +[](LSMap<LSValue*, LSValue*>* map, LSValue* key) {
					return map->atL_base(key);
				});
			}
		} else {
			k = c.insn_to_any(k);
			return c.insn_invoke(type.pointer(), {compiled_array, k}, (void*) +[](LSValue* array, LSValue* key) {
				return array->atL(key);
			});
		}
	} else {
		auto start = key->compile(c);
		auto end = key2->compile(c);
		key->compile_end(c);
		key2->compile_end(c);
		c.mark_offset(open_bracket->location.start.line);
		return c.insn_call(Type::any(), {compiled_array, start, end}, (void*) +[](LSValue* a, int start, int end) {
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
