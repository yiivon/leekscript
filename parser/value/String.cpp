#include "String.hpp"
#include "../../vm/VM.hpp"

using namespace std;

String::String(string value) {
	this->value = value;
	type = Type::STRING;
	constant = true;
}

String::~String() {}

void String::print(ostream& os) const {
	os << "'";
	os << value;
	os << "'";
}

void String::analyse(SemanticAnalyser* analyser, const Type) {

}

jit_value_t String::compile_jit(Compiler&, jit_function_t& F, Type type) const {

	LSString* s = new LSString(value);
	return JIT_CREATE_CONST_POINTER(F,  s);
}
