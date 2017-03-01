#include "../../compiler/value/String.hpp"
#include "../../vm/value/LSString.hpp"

using namespace std;

namespace ls {

String::String(string& value, Token* token) {
	this->value = value;
	this->token = token;
	type = Type::STRING;
	types = Type::STRING;
	type.temporary = true;
	constant = true;
	ls_string = new LSString(value);
}

String::~String() {
	delete ls_string;
}

void String::print(ostream& os, int, bool debug) const {
	os << "'" << value << "'";
	if (debug) {
		os << " " << type;
	}
}

unsigned String::line() const {
	return token->line;
}

void String::analyse(SemanticAnalyser*, const Type&) {
	// Nothing to do, always a pointer
}

Compiler::value String::compile(Compiler& c) const {
	auto base = c.new_pointer(ls_string);
	return c.insn_call(Type::STRING, {base}, (void*) +[](LSString* s) {
		return s->clone();
	});
}

}
