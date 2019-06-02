#ifndef STRING_HPP_
#define STRING_HPP_

#include <string>
#include "Value.hpp"
#include "../lexical/Token.hpp"
#include "../../vm/value/LSString.hpp"

namespace ls {

class String : public Value {
public:

	std::shared_ptr<Token> token;

	String(std::shared_ptr<Token> token);
	virtual ~String();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual bool will_store(SemanticAnalyzer* analyzer, const Type* type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
