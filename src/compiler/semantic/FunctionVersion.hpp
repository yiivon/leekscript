#ifndef FUNCTION_VERSION_HPP
#define FUNCTION_VERSION_HPP

#include "../Compiler.hpp"

namespace ls {

class Block;
class Function;
class SemanticAnalyzer;
class Variable;

class FunctionVersion {
public:
	Function* parent;
	std::unique_ptr<Block> body;
	const Type* type;
	llvm::BasicBlock* block = nullptr;
	llvm::Function* f = nullptr;
	Compiler::value value;
	const Type* placeholder_type = nullptr;
	bool recursive = false;
	std::map<std::string, Variable*> arguments;

	FunctionVersion(std::unique_ptr<Block> body);

	bool is_compiled() const;

	void print(std::ostream& os, int indent, bool debug, bool condensed) const;

	const Type* getReturnType();
	void pre_analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args);
	void analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args);

	void create_function(Compiler& c);
	void compile(Compiler& c, bool create_value = false, bool compile_body = true);
	void compile_return(const Compiler& c, Compiler::value v, bool delete_variables = false) const;
	llvm::BasicBlock* get_landing_pad(const Compiler& c);
};

}

#endif