#ifndef NULLSTD_HPP
#define NULLSTD_HPP

#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../VM.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class LSNull;

class NullSTD : public Module {
public:
	NullSTD();
};

}

#endif
