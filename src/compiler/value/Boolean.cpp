#include "Boolean.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/value/LSBoolean.hpp"

using namespace std;

namespace ls {

Boolean::Boolean(std::shared_ptr<Token> token) {
	this->token = token;
	this->value = token->type == TokenType::TRUE;
	type = Type::BOOLEAN;
	ty = std::make_shared<LType>(new Bool_type());
	constant = true;
}

void Boolean::print(std::ostream& os, int, bool debug, bool condensed) const {
	os << (value ? "true" : "false");
	if (debug) {
		os << " " << ty;
	}
}

Location Boolean::location() const {
	return token->location;
}

void Boolean::analyse(SemanticAnalyser*, const Type& req_type) {}

Compiler::value Boolean::compile(Compiler& c) const {
	return c.new_bool(value);
}

Value* Boolean::clone() const {
	auto b = new Boolean(token);
	b->value = value;
	return b;
}

}
