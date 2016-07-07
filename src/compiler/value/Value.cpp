#include "Value.hpp"
#include "../../vm/Type.hpp"

namespace ls {

Value::Value() {
	type = Type::NEUTRAL;
	constant = false;
}

Value::~Value() {}

void Value::analyse(SemanticAnalyser* analyser) {
	analyse(analyser, Type::NEUTRAL);
}

bool Value::will_take(SemanticAnalyser*, const unsigned i, const Type arg_type) {
	return type.will_take(i, arg_type);
}

bool Value::will_take_element(SemanticAnalyser*, const Type arg_type) {
	return type.will_take_element(arg_type);
}

bool Value::must_be_pointer(SemanticAnalyser*) {
	if (type.nature == Nature::POINTER) {
		return false;
	}
	type.nature = Nature::POINTER;
	return true;
}

bool Value::isLeftValue() const {
	return false;
}

}
