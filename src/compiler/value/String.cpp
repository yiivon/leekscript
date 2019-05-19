#include "String.hpp"
#include "../../vm/value/LSString.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

String::String(std::shared_ptr<Token> token) : token(token) {
	type = Type::string();
	type.temporary = true;
	constant = true;
}

String::~String() {}

void String::print(std::ostream& os, int, bool debug, bool condensed) const {
	os << "'" << token->content << "'";
	if (debug) {
		os << " " << type;
	}
}

Location String::location() const {
	return token->location;
}

bool String::will_store(SemanticAnalyzer* analyzer, const Type& type) {
	if (!type.is_string()) {
		analyzer->add_error({SemanticError::Type::NO_SUCH_OPERATOR, location(), location(), {this->type.to_string(), "=", type.to_string()}});
	}
	return false;
}

Compiler::value String::compile(Compiler& c) const {
	auto s = c.new_const_string(token->content, "string");
	return c.insn_call(Type::tmp_string(), {s}, "String.new.1");
}

Value* String::clone() const {
	return new String(token);
}

}
