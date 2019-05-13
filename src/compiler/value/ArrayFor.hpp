#ifndef ARRAYFOR_HPP
#define ARRAYFOR_HPP

#include "Value.hpp"
#include "../instruction/For.hpp"

namespace ls {

class ArrayFor : public Value {
public:
	Instruction* forr;

	virtual ~ArrayFor();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyze(SemanticAnalyzer*) override;
	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}
#endif // ARRAYFOR_H
