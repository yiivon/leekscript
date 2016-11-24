#ifndef VALUE_STD_HPP
#define VALUE_STD_HPP

#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../VM.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class ValueSTD : public Module {
public:
	ValueSTD();

	static bool instanceof_ptr(LSValue* x, LSClass* c);
	static Compiler::value and_value_value(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value or_value_value(Compiler& c, std::vector<Compiler::value> args);
	/*
	 * Methods
	 */
	static Compiler::value to_json(Compiler& c, std::vector<Compiler::value> args);
};

}

#endif
