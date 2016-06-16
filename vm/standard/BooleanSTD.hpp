#ifndef BOOLEAN_STD_HPP
#define BOOLEAN_STD_HPP

#include "../VM.hpp"
#include "../../parser/semantic/SemanticAnalyser.hpp"
#include "../../parser/Program.hpp"
#include "../Module.hpp"

namespace ls {

class BooleanSTD : public Module {
public:
	BooleanSTD();
};

int boolean_compare(bool, bool);

}

#endif
