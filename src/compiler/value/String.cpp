#include "../../compiler/value/String.hpp"

#include "../../vm/value/LSString.hpp"

using namespace std;

namespace ls {

String::String(string& value, Token* token) {
	this->value = value;
	this->token = token;
	type = Type::STRING;
	constant = true;
	ls_string = new LSString(value);
}

String::~String() {
	LSValue::delete_val(ls_string);
}

void String::print(ostream& os, bool) const {
	os << "'";
	os << value;
	os << "'";
}

unsigned String::line() const {
	return token->line;
}

void String::analyse(SemanticAnalyser*, const Type&) {
	// Nothing to do, always a pointer
}

LSValue* String_create(LSString* s) {
	return s->clone();
}

jit_value_t String::compile(Compiler& c) const {

	jit_value_t base = JIT_CREATE_CONST_POINTER(c.F, ls_string);

	jit_type_t args_types[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);

	return jit_insn_call_native(c.F, "create", (void*) String_create, sig, &base, 1, JIT_CALL_NOTHROW);
}

}
