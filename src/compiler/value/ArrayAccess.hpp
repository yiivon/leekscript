#ifndef ARRAYACCESS_HPP
#define ARRAYACCESS_HPP

#include <memory>
#include "LeftValue.hpp"
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class ArrayAccess : public LeftValue {
public:

	Value* array;
	Value* key;
	Value* key2;
	std::shared_ptr<Token> open_bracket;
	std::shared_ptr<Token> close_bracket;
	Type map_key_type;
	Compiler::value compiled_array;

	ArrayAccess();
	virtual ~ArrayAccess();
	
	virtual bool isLeftValue() const override;

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual Callable* get_callable(SemanticAnalyzer*, int argument_count) const override;
	virtual void analyze(SemanticAnalyzer*) override;
	virtual bool will_take(SemanticAnalyzer* analyzer, const std::vector<Type>&, int level);
	bool array_access_will_take(SemanticAnalyzer* analyzer, const std::vector<Type>&, int level);
	virtual bool will_store(SemanticAnalyzer* analyzer, const Type& type) override;
	virtual void change_value(SemanticAnalyzer*, Value*) override;
	virtual Type version_type(std::vector<Type>) const override;

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_l(Compiler&) const override;
	virtual void compile_end(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
