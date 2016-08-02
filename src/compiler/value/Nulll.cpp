#include "../../compiler/value/Nulll.hpp"

#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Nulll::Nulll() {
	type = Type::NULLL;
	constant = true;
}

Nulll::~Nulll() {}

void Nulll::print(ostream& os, bool debug) const {
	os << "null";
}

int Nulll::line() const {
	return 0;
}

void Nulll::analyse(SemanticAnalyser*, const Type&) {
	// nothing to do, always a pointer
}

jit_value_t Nulll::compile(Compiler& c) const {
	return VM::create_null(c.F);
}

}
