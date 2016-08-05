#include "../../compiler/instruction/ClassDeclaration.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSClass.hpp"

using namespace std;

namespace ls {

ClassDeclaration::ClassDeclaration() {
	can_return = false;
}

ClassDeclaration::~ClassDeclaration() {}

void ClassDeclaration::print(ostream& os, int indent, bool debug) const {
	os << "class " << name << " {" << endl;
	for (VariableDeclaration* vd : fields) {
		vd->print(os, indent + 1, debug);
		os << endl;
	}
	os << "}";
}

void ClassDeclaration::analyse(SemanticAnalyser* analyser, const Type&) {
	for (VariableDeclaration* vd : fields) {
		vd->analyse(analyser, Type::UNKNOWN);
	}
}

jit_value_t ClassDeclaration::compile(Compiler&) const {
	return nullptr;
}

}
