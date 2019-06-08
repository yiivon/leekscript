#ifndef FUNCTION_VERSION_HPP
#define FUNCTION_VERSION_HPP

#include "../Compiler.hpp"

namespace ls {

class Block;
class Function;

class FunctionVersion {
public:
	Function* parent;
	Block* body;
	const Type* type;
	llvm::BasicBlock* landing_pad = nullptr;
	llvm::BasicBlock* catch_block = nullptr;
	llvm::AllocaInst* exception_slot = nullptr;
	llvm::AllocaInst* exception_line_slot = nullptr;
	llvm::BasicBlock* block = nullptr;
	llvm::Function* f = nullptr;
	Compiler::value value;

	FunctionVersion();
	bool is_compiled() const;
	void create_function(Compiler& c);
	void compile(Compiler& c, bool create_value = false, bool compile_body = true);
};

}

#endif