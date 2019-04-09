#include "String.hpp"
#include "../../vm/value/LSString.hpp"
#include "../semantic/SemanticAnalyser.hpp"

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

bool String::will_store(SemanticAnalyser* analyser, const Type& type) {
	if (!type.is_string()) {
		analyser->add_error({SemanticError::Type::NO_SUCH_OPERATOR, location(), location(), {this->type.to_string(), "=", type.to_string()}});
	}
	return false;
}

Compiler::value String::compile(Compiler& c) const {
	auto s = c.builder.CreateGlobalStringPtr(token->content, "str");
	return c.insn_call(Type::tmp_string(), {{s, Type::integer().pointer()}}, (void*) +[](char* s) {
		return new LSString(std::string(s));
	}, "String::new");
}

Value* String::clone() const {
	return new String(token);
}

}
