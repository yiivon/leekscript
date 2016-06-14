#include "Compiler.hpp"

namespace ls {

Compiler::Compiler() {}

Compiler::~Compiler() {}

void Compiler::enter_loop(jit_label_t* end_label, jit_label_t* cond_label) {
	loops_end_labels.push_back(end_label);
	loops_cond_labels.push_back(cond_label);
}

void Compiler::leave_loop() {
	loops_end_labels.pop_back();
	loops_cond_labels.pop_back();
}

jit_label_t* Compiler::get_current_loop_end_label() const {
	return loops_end_labels.back();
}

jit_label_t* Compiler::get_current_loop_cond_label() const {
	return loops_cond_labels.back();
}

}
