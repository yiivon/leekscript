#ifndef FUNCTION_VERSION_HPP
#define FUNCTION_VERSION_HPP

#include "../Compiler.hpp"

namespace ls {

class Block;
class Function;
class SemanticAnalyzer;

class FunctionVersion {
public:
	Function* parent;
	Block* body;
	const Type* type;
	llvm::BasicBlock* block = nullptr;
	llvm::Function* f = nullptr;
	Compiler::value value;

	FunctionVersion();

	bool is_compiled() const;

	void print(std::ostream& os, int indent, bool debug, bool condensed) const;

	void analyze(SemanticAnalyzer* analyzer, std::vector<const Type*> args);

	void create_function(Compiler& c);
	void compile(Compiler& c, bool create_value = false, bool compile_body = true);
};

}

#endif