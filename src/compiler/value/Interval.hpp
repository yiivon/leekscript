#ifndef INTERVAL_HPP
#define INTERVAL_HPP

#include <vector>
#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Interval : public Value {
public:

	std::shared_ptr<Token> opening_bracket;
	std::shared_ptr<Token> closing_bracket;
	Value* start;
	Value* end;

	Interval();
	virtual ~Interval();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
