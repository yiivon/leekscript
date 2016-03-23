#ifndef FOR_HPP
#define FOR_HPP

#include <vector>
#include "../lexical/Ident.hpp"
#include "../value/Value.hpp"
#include "../Body.hpp"
#include "../value/Expression.hpp"
#include "../semantic/SemanticAnalyser.hpp"
class SemanticVar;

class For : public Instruction {
public:

	std::vector<Token*> variables;
	std::vector<Value*> variablesValues;
	std::vector<bool> declare_variables;
	Value* condition;
	std::vector<Value*> iterations;
	Body* body;
	std::map<std::string, SemanticVar*> vars;

	For();
	virtual ~For();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

#endif
