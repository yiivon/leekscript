#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include "Expression.hpp"
#include "../lexical/Ident.hpp"

namespace ls {

class Object : public Value {
public:

	std::vector<std::shared_ptr<Token>> keys;
	std::vector<Value*> values;

	Object();
	virtual ~Object();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
