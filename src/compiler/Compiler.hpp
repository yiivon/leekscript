#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <jit/jit.h>
#include <vector>
#include <map>

#include "../vm/Type.hpp"

namespace ls {

class CompilerVar {
public:
	jit_value_t value;
	Type type;
	bool reference;
	CompilerVar() : value(jit_value_t{}), type(Type::UNKNOWN), reference(false) {};
	CompilerVar(jit_value_t& value, const Type& type, bool reference) :
		value(value), type(type), reference(reference) {};
};

class Compiler {
public:

	std::vector<jit_label_t*> loops_end_labels;
	std::vector<jit_label_t*> loops_cond_labels;
	std::vector<std::map<std::string, CompilerVar>> variables;

	Compiler();
	virtual ~Compiler();

	void enter_block();
	void leave_block(jit_function_t& F);

	void add_var(std::string& name, jit_value_t& value, const Type& type, bool ref);
	CompilerVar& get_var(std::string& name);
	void set_var_type(std::string& name, const Type& type);
	std::map<std::string, CompilerVar> get_vars();

	void enter_loop(jit_label_t*, jit_label_t*);
	void leave_loop();
	jit_label_t* get_current_loop_end_label() const;
	jit_label_t* get_current_loop_cond_label() const;
};

}

#endif
