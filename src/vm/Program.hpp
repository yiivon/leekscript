#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "../compiler/value/Block.hpp"
#include "../compiler/value/Function.hpp"
#include "../compiler/semantic/SemanticAnalyser.hpp"

namespace ls {

class LSValue;

class Program {

public:

	std::vector<Function*> functions;
	std::map<std::string, LSValue*> system_vars;
	Function* main;
	void* closure;

	Program();
	virtual ~Program();

	void compile(Context&);
	LSValue* execute();
	void compile_jit(Compiler&, Context&, bool);

	void print(std::ostream& os, bool debug = false) const;
};

std::ostream& operator << (std::ostream& os, const Program* program);

}

#endif
