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
};

}

#endif
