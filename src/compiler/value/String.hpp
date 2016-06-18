#ifndef STRING_HPP_
#define STRING_HPP_

#include <string>

#include "../../compiler/value/Value.hpp"
#include "../lexical/Token.hpp"
#include "../../vm/value/LSString.hpp"

namespace ls {

class String : public Value {
public:

	std::string value;
	LSString* ls_string;

	String(std::string value);
	virtual ~String();

	virtual void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
