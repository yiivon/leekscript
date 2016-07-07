#ifndef FOR_HPP
#define FOR_HPP

#include <vector>

#include "../../compiler/lexical/Ident.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../compiler/value/Expression.hpp"
#include "../../compiler/value/Value.hpp"
#include "Instruction.hpp"

namespace ls {

class Block;
class SemanticVar;

class For : public Instruction {
public:

	std::vector<Token*> variables;
	std::vector<Value*> variablesValues;
	std::vector<bool> declare_variables;
	Value* condition;
	std::vector<Value*> iterations;
	Block* body;
	std::map<std::string, SemanticVar*> vars;

	For();
	virtual ~For();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
