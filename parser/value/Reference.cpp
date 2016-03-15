#include "Reference.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"

using namespace std;

Reference::Reference() {
	type = Type::VALUE;
}

Reference::~Reference() {}

void Reference::print(ostream& os) const {
	os << "@" << variable;
}

void Reference::analyse(SemanticAnalyser* analyser, const Type) {

}

jit_value_t Reference::compile_jit(Compiler& c, jit_function_t& F, Type type) const {

	if (type.nature == Nature::VALUE) {
		LSNumber* n = new LSNumber(0);
		return jit_value_create_nint_constant(F, jit_type_void_ptr, (long int) n);
	} else {
		return jit_value_create_nint_constant(F, jit_type_void_ptr, 0);
	}
}
