#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "../compiler/value/Function.hpp"
#include "VM.hpp"

namespace ls {

class Program {
private:

	std::string code; // The program code
	void* closure;

public:

	Function* main;
	Type type;
	std::vector<Function*> functions;
	std::string file_name;
	VM* vm;
	bool handle_created = false;
	llvm::Module* module = nullptr;
	llvm::orc::VModuleKey module_handle;

	Program(const std::string& code, const std::string& file_name);
	virtual ~Program();

	void analyse(SemanticAnalyser* analyser);

	/*
	 * Compile the program with a VM and a context (json)
	 */
	VM::Result compile(VM& vm, const std::string& context, bool assembly = false, bool pseudo_code = false, bool log_instructions = false, bool ir = false);

	VM::Result compile_leekscript(VM& vm, const std::string& ctx, bool assembly, bool pseudo_code, bool log_instructions);

	/*
	 * Execute the program and get a std::string result
	 */
	std::string execute(VM& vm);

	void print(std::ostream& os, bool debug = false) const;

	std::string underline_code(Location location, Location focus) const;
};

std::ostream& operator << (std::ostream& os, const Program* program);

}

#endif
