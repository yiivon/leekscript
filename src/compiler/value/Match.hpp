#ifndef MATCH_HPP_
#define MATCH_HPP_

#include "Value.hpp"
#include "Block.hpp"
#include <vector>

namespace ls {

class Match : public Value {
public:
	class Pattern {
	public:
		bool interval;
		Value* begin;
		Value* end;

		Pattern(Value* value);
		Pattern(Value* begin, Value* end);
		~Pattern();

		inline bool is_default() const { return !begin && !end; }

		void print(std::ostream&, int indent, bool debug) const;
		Compiler::value match(Compiler &c, Compiler::value v) const;

		Pattern clone() const {
			Pattern p { begin->clone(), end->clone() };
			p.interval = interval;
			return p;
		}
	};

	Value* value;
	std::vector<std::vector<Pattern>> pattern_list;
	std::vector<Value*> returns;

	Match();
	virtual ~Match();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*) override;
	Compiler::value construct_branch(Compiler& c, Compiler::value v, size_t i) const;
	Compiler::value get_pattern_condition(Compiler& c, Compiler::value v, std::vector<Pattern>) const;
	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
