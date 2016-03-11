#include "ClassDeclaration.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSClass.hpp"

using namespace std;

ClassDeclaration::ClassDeclaration() {}

ClassDeclaration::~ClassDeclaration() {}

void ClassDeclaration::print(ostream& os) const {
	os << "class " << name << " {" << endl;
	for (VariableDeclaration* vd : fields) {
		vd->print(os);
		os << endl;
	}
	os << "}";
}

void ClassDeclaration::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	for (VariableDeclaration* vd : fields) {
		vd->analyse(analyser, Type::UNKNOWN);
	}
}

jit_value_t ClassDeclaration::compile_jit(Compiler& c, jit_function_t& F, Type) const {
	return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
}
