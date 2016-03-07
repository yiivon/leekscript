#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "Body.hpp"
#include "value/Function.hpp"
#include "semantic/SemanticAnalyser.hpp"

class Program {

public:

	vector<Function*> functions;
	map<string, SemanticVar*> global_vars;
	map<string, LSValue*> system_vars;
	Body* body;

	Program();
	virtual ~Program();

	void print(ostream& os);

	void compile_jit(Compiler&, jit_function_t&, Context&, bool);
};

#endif
