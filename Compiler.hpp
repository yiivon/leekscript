#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <jit/jit.h>
#include <vector>

class Compiler {
public:

	std::vector<jit_label_t*> loops_end_labels;
	std::vector<jit_label_t*> loops_cond_labels;

	Compiler();
	virtual ~Compiler();

	void enter_loop(jit_label_t*, jit_label_t*);
	void leave_loop();

	jit_label_t* get_current_loop_end_label() const;
	jit_label_t* get_current_loop_cond_label() const;
};

#endif
