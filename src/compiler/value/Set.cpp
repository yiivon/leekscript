#include "Set.hpp"
#include "../../vm/value/LSSet.hpp"
#include "../../vm/VM.hpp"
#include "../../type/RawType.hpp"

using namespace std;

namespace ls {

Set::Set() {}

Set::~Set() {
	for (auto ex : expressions) delete ex;
}

void Set::print(ostream& os, int indent, bool debug, bool condensed) const {
	os << "<";
	for (size_t i = 0; i < expressions.size(); ++i) {
		if (i > 0) os << ", ";
		expressions[i]->print(os, indent + 1, debug);
	}
	os << ">";
	if (debug) os << " " << type;
}

Location Set::location() const {
	return {{0, 0, 0}, {0, 0, 0}}; // TODO
}

void Set::analyse(SemanticAnalyser* analyser) {

	Type element_type = {};

	for (auto& ex : expressions) {
		ex->analyse(analyser);
		element_type = element_type * ex->type;
	}

	if (element_type.is_primitive()) {
		if (element_type != Type::INTEGER && element_type != Type::REAL) {
			element_type = Type::ANY;
		}
	} else if (!element_type.is_primitive()) {
		element_type = Type::ANY;
	}

	constant = true;
	for (auto& ex : expressions) {
		ex->analyse(analyser);
		constant = constant && ex->constant;
	}
	if (element_type._types.size() == 0) {
		element_type = Type::ANY;
	}
	type = Type::set(element_type);
}

bool Set::will_store(SemanticAnalyser* analyser, const Type& type) {

	Type added_type = type;
	if (added_type.is_array() or added_type.is_set()) {
		added_type = added_type.element();
	}
	Type current_type = this->type.element();
	if (expressions.size() == 0) {
		this->type = Type::set(added_type);
	} else {
		this->type = Type::set(current_type * added_type);
	}
	// Re-analyze expressions with the new type
	for (size_t i = 0; i < expressions.size(); ++i) {
		expressions[i]->analyse(analyser);
	}
	return false;
}

LSSet<LSValue*>* Set_create_ptr() { return new LSSet<LSValue*>(); }
LSSet<int>* Set_create_int()      { return new LSSet<int>();      }
LSSet<double>* Set_create_float() { return new LSSet<double>();   }

void Set_insert_ptr(LSSet<LSValue*>* set, LSValue* value) {
	auto it = set->lower_bound(value);
	if (it == set->end() || (**it != *value)) {
		set->insert(it, value->move_inc());
	}
	LSValue::delete_temporary(value);
}
void Set_insert_int(LSSet<int>* set, int value) {
	set->insert(value);
}
void Set_insert_float(LSSet<double>* set, double value) {
	set->insert(value);
}

Compiler::value Set::compile(Compiler& c) const {
	void* create = type.element() == Type::INTEGER ? (void*) Set_create_int :
				   type.element() == Type::REAL    ? (void*) Set_create_float : (void*) Set_create_ptr;
	void* insert = type.element() == Type::INTEGER ? (void*) Set_insert_int :
				   type.element() == Type::REAL    ? (void*) Set_insert_float : (void*) Set_insert_ptr;

	unsigned ops = 1;
	auto s = c.insn_call(type, {}, (void*) create);

	double i = 0;
	for (Value* ex : expressions) {
		auto v = c.insn_convert(ex->compile(c), type.element());
		ex->compile_end(c);
		c.insn_call({}, {s, v}, (void*) insert);
		ops += std::log2(++i);
	}
	c.inc_ops(ops);
	return s;
}

Value* Set::clone() const {
	auto s = new Set();
	for (const auto& v : expressions) {
		s->expressions.push_back(v->clone());
	}
	return s;
}

}
