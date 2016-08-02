#include "../../compiler/value/Nulll.hpp"

#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Nulll::Nulll() {
	type = Type::NULLL;
	constant = true;
}

Nulll::~Nulll() {}

void Nulll::print(ostream& os) const {
	os << "null";
}

unsigned Nulll::line() const {
	return 0;
}

void Nulll::analyse(SemanticAnalyser*, const Type) {
	// nothing to do
}

jit_value_t Nulll::compile_jit(Compiler&, jit_function_t& F, Type) const {
	return VM::create_null(F);
}

}
