#include "Nulll.hpp"
#include "../../vm/VM.hpp"

using namespace std;

Nulll::Nulll() {
	type = Type::NULLL;
	constant = true;
}

Nulll::~Nulll() {}

void Nulll::print(ostream& os) const {
	os << "null";
}

void Nulll::analyse(SemanticAnalyser* analyser, const Type) {

}

jit_value_t Nulll::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	if (req_type.nature == Nature::POINTER) {
		LSNull* n = new LSNull();
		return JIT_CREATE_CONST_POINTER(F, n);
	} else {
		return JIT_CREATE_CONST(F, JIT_INTEGER, 0);
	}
}
