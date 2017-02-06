#include "../../compiler/value/Object.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSObject.hpp"

using namespace std;

namespace ls {

Object::Object() {
	type = Type::OBJECT;
}

Object::~Object() {
	for (auto key : keys) {
		delete key;
	}
	for (auto ex : values) {
		delete ex;
	}
}

void Object::print(ostream& os, int indent, bool debug) const {
	os << "{";
	for (unsigned i = 0; i < keys.size(); ++i) {
		os << keys.at(i)->token->content;
		os << ": ";
		values.at(i)->print(os, indent);
		if (i < keys.size() - 1) {
			os << ", ";
		}
	}
	os << "}";
	if (debug) {
		os << " " << type;
	}
}

void Object::analyse(SemanticAnalyser* analyser, const Type&) {
	for (Value* value : values) {
		value->analyse(analyser, Type::POINTER);
	}
}

Compiler::value Object::compile(Compiler& c) const {

	Compiler::value object = {VM::create_object(c.F), Type::OBJECT};

	for (unsigned i = 0; i < keys.size(); ++i) {
		auto k = c.new_pointer((void*) &keys.at(i).content);
		auto v = values[i]->compile(c);
		c.insn_call(Type::VOID, {object, k, v}, +[](LSObject* o, std::string* k, LSValue* v) {
			o->addField(*k, v);
		});
	}
	return object;
}

}
