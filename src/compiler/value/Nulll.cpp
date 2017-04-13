#include "Nulll.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Nulll::Nulll(Token* token) {
	type = Type::NULLL;
	constant = true;
	this->token.reset(token);
}

void Nulll::print(ostream& os, int, bool debug) const {
	os << "null";
	if (debug) {
		os << " " << type;
	}
}

void Nulll::analyse(SemanticAnalyser*, const Type&) {
	// nothing to do, always a pointer
}

Compiler::value Nulll::compile(Compiler& c) const {
	return c.new_null();
}

}
