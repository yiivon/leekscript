#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "../compiler/value/Function.hpp"

namespace ls {

class SemanticAnalser;
class LSValue;

class Program {
private:

	std::string code; // The program code
	void* closure;

	void compile_main(VM& vm, Context&);
	void compile_jit(VM& vm, Compiler&, Context&, bool);

public:

	Function* main;
	std::vector<Function*> functions;

	Program(const std::string& code);
	virtual ~Program();

	void analyse(SemanticAnalyser* analyser);

	/*
	 * Compile the program with a VM and a context (json)
	 */
	VM::Result compile(VM& vm, const std::string& context);

	/*
	 * Execute the program and get a LSValue* result
	 */
	std::string execute();

	void print(std::ostream& os, bool debug = false) const;
};

std::ostream& operator << (std::ostream& os, const Program* program);

}

#endif
