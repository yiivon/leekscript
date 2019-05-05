#include "Map.hpp"
#include "../../vm/value/LSMap.hpp"
#include <cmath>

namespace ls {

Map::Map() {

}

Map::~Map() {
	for (auto ex : keys) {
		delete ex;
	}
	for (auto ex : values) {
		delete ex;
	}
}

void Map::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	if (values.empty()) {
		os << "[:]";
	} else {
		os << "[";
		for (size_t i = 0; i < values.size(); ++i) {
			if (i > 0) os << ", ";
			keys[i]->print(os, indent + 1, debug);
			os << ": ";
			values[i]->print(os, indent + 1, debug);
		}
		os << "]";
	}
	if (debug) {
		os << " " << type;
	}
}

Location Map::location() const {
	return {opening_bracket->location.start, closing_bracket->location.end};
}

void Map::analyse(SemanticAnalyser* analyser) {

	Type key_type = {};
	Type value_type = {};

	for (size_t i = 0; i < keys.size(); ++i) {
		Value* ex = keys[i];
		ex->analyse(analyser);
		key_type = key_type * ex->type;
		if (ex->constant == false) constant = false;
	}
	key_type.temporary = false;

	for (size_t i = 0; i < values.size(); ++i) {
		Value* ex = values[i];
		ex->analyse(analyser);
		value_type = value_type * ex->type;
		if (ex->constant == false) constant = false;
	}
	value_type.temporary = false;

	if (key_type == Type::integer() or key_type == Type::real()) {
	} else {
		key_type = Type::any();
		// key_type.setReturnType(Type::any());
	}
	if (value_type == Type::integer() || value_type == Type::real()) {
	} else {
		value_type = Type::any();
		// value_type.setReturnType(Type::any());
	}
	type = Type::map(key_type, value_type);
	type.temporary = true;
}

void LSMap_insert_ptr_ptr(LSMap<LSValue*, LSValue*>* map, LSValue* key, LSValue* value) {
	auto it = map->lower_bound(key);
	if (it == map->end() || *it->first != *key) {
		map->emplace_hint(it, key->move_inc(), value->move_inc());
	}
}
void LSMap_insert_ptr_int(LSMap<LSValue*, int>* map, LSValue* key, int value) {
	auto it = map->lower_bound(key);
	if (it == map->end() || *it->first != *key) {
		map->emplace_hint(it, key->move_inc(), value);
	}
}
void LSMap_insert_ptr_float(LSMap<LSValue*, double>* map, LSValue* key, double value) {
	auto it = map->lower_bound(key);
	if (it == map->end() || *it->first != *key) {
		map->emplace_hint(it, key->move_inc(), value);
	}
}
void LSMap_insert_int_ptr(LSMap<int, LSValue*>* map, int key, LSValue* value) {
	auto it = map->lower_bound(key);
	if (it == map->end() || it->first != key) {
		map->emplace_hint(it, key, value->move_inc());
	}
}
void LSMap_insert_int_int(LSMap<int, int>* map, int key, int value) {
	map->emplace(key, value);
}
void LSMap_insert_int_float(LSMap<int, double>* map, int key, double value) {
	map->emplace(key, value);
}
void LSMap_insert_real_ptr(LSMap<double, LSValue*>* map, double key, LSValue* value) {
	auto it = map->lower_bound(key);
	if (it == map->end() || it->first != key) {
		map->emplace_hint(it, key, value->move_inc());
	}
}
void LSMap_insert_real_int(LSMap<double, int>* map, double key, int value) {
	map->emplace(key, value);
}
void LSMap_insert_real_float(LSMap<double, double>* map, double key, double value) {
	map->emplace(key, value);
}

Compiler::value Map::compile(Compiler &c) const {

	std::string create;
	void* insert = nullptr;

	if (type.key() == Type::integer()) {
		create = type.element().is_integer() ? "Map.new.8" : type.element().is_real() ? "Map.new.7" : "Map.new.6";
		insert = type.element() == Type::integer() ? (void*) LSMap_insert_int_int :
				 type.element() == Type::real()   ? (void*) LSMap_insert_int_float
						 	 	 	 	 	 	 	    : (void*) LSMap_insert_int_ptr;
	} else if (type.key().is_real()) {
		create = type.element().is_integer() ? "Map.new.5" : type.element().is_real() ? "Map.new.4" : "Map.new.3";
		insert = type.element() == Type::integer() ? (void*) LSMap_insert_real_int :
				 type.element() == Type::real()   ? (void*) LSMap_insert_real_float
													    : (void*) LSMap_insert_real_ptr;
	} else {
		create = type.element().is_integer() ? "Map.new.2" : type.element().is_real() ? "Map.new.1" : "Map.new";
		insert = type.element() == Type::integer() ? (void*) LSMap_insert_ptr_int :
				 type.element() == Type::real()   ? (void*) LSMap_insert_ptr_float
						 	 	 	 	 	 	 	    : (void*) LSMap_insert_ptr_ptr;
	}

	unsigned ops = 0;
	auto map = c.insn_call(type, {}, create);

	for (size_t i = 0; i < keys.size(); ++i) {
		auto k = c.insn_convert(keys[i]->compile(c), type.key());
		keys[i]->compile_end(c);
		auto v = c.insn_convert(values[i]->compile(c), type.element());
		values[i]->compile_end(c);

		c.insn_call({}, {map, k, v}, (void*) insert);
		ops += std::log2(i + 1);

		c.insn_delete_temporary(k);
		c.insn_delete_temporary(v);
	}
	c.inc_ops(ops);
	return map;
}

Value* Map::clone() const {
	auto map = new Map();
	map->opening_bracket = opening_bracket;
	map->closing_bracket = closing_bracket;
	for (const auto& k : keys) {
		map->keys.push_back(k->clone());
	}
	for (const auto& v : values) {
		map->values.push_back(v->clone());
	}
	return map;
}

}
