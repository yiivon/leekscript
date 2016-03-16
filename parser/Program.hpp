#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "Body.hpp"
#include "value/Function.hpp"
#include "semantic/SemanticAnalyser.hpp"

class Program {

public:

	std::vector<Function*> functions;
	std::map<std::string, SemanticVar*> global_vars;
	std::map<std::string, LSValue*> system_vars;
	Body* body;

	Program();
	virtual ~Program();

	void print(std::ostream& os);

	void compile_jit(Compiler&, jit_function_t&, Context&, bool);
};

#endif
