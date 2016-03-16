#include "Return.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"

using namespace std;

Return::Return() {
	expression = nullptr;
	function = nullptr;
	in_function = false;
}

Return::Return(Value* v) {
	expression = v;
	function = nullptr;
	in_function = false;
}

Return::~Return() {

}

void Return::print(ostream& os) const {
	os << "return ";
	expression->print(os);
}

void Return::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	expression->analyse(analyser, req_type);
	Function* f = analyser->current_function();
	if (f != nullptr) {
		function = f;
		in_function = true;
	}
	type = expression->type;
}

jit_value_t Return::compile_jit(Compiler& c, jit_function_t& F, Type type) const {

//	if (info.return_nature == Nature::POINTER) {
//		info.nature = Nature::POINTER;
//	}
	jit_value_t v = expression->compile_jit(c, F, type);
	jit_insn_return(F, v);
	return v;
}
