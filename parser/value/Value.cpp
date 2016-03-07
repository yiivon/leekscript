#include "Value.hpp"

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

bool Value::isLeftValue() const {
	return false;
}

