#include <sstream>
#include "Value.hpp"
#include "../../type/Type.hpp"
#include "../semantic/Callable.hpp"

namespace ls {

Value::Value() {
	type = Type::any();
	constant = false;
}

Value::~Value() {}

Callable* Value::get_callable(SemanticAnalyser*) const {
	auto callable = new Callable("?");
	callable->add_version({ "?", type, this, {}, {}, nullptr, true });
	return callable;
}

void Value::analyse(SemanticAnalyser*) {}

bool Value::will_take(SemanticAnalyser*, const std::vector<Type>& args, int) {
	set_version(args, 1);
	return false;
}

bool Value::will_store(SemanticAnalyser*, const Type&) {
	return false;
}

bool Value::elements_will_store(SemanticAnalyser*, const Type&, int level) {
	return false;
}

bool Value::must_be_any(SemanticAnalyser*) {
	if (type == Type::any()) {
		return false;
	}
	type = Type::any();
	return true;
}

void Value::must_return(SemanticAnalyser*, const Type& ret_type) {
	// TODO
	// type.setReturnType(ret_type);
}

void Value::will_be_in_array(SemanticAnalyser*) {}

void Value::set_version(const std::vector<Type>& args, int) {
	version = args;
	has_version = true;
}

Type Value::version_type(std::vector<Type>) const {
	return type;
}

bool Value::isLeftValue() const {
	return false;
}

bool Value::is_zero() const {
	return false;
}

Compiler::value Value::compile_version(Compiler& c, std::vector<Type>) const {
	return compile(c);
}

std::string Value::tabs(int indent) {
	return std::string(indent * 4, ' ');
}

std::string Value::to_string() const {
	std::ostringstream oss;
	print(oss, 0, false, true);
	return oss.str();
}

std::ostream& operator << (std::ostream& os, const Value* v) {
	v->print(os, 0, false, true);
	return os;
}

}
