#include "Nulll.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Nulll::Nulll(std::shared_ptr<Token> token) : token(token) {
	type = Type::null();
	constant = true;
}

void Nulll::print(ostream& os, int, bool debug, bool condensed) const {
	os << "null";
	if (debug) {
		os << " " << type;
	}
}

Location Nulll::location() const {
	return token->location;
}

Compiler::value Nulll::compile(Compiler& c) const {
	return c.new_null();
}

Value* Nulll::clone() const {
	return new Nulll(token);
}

}
