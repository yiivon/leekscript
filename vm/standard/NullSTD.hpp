#ifndef NULLSTD_HPP
#define NULLSTD_HPP

#include "../VM.hpp"
#include "../../parser/semantic/SemanticAnalyser.hpp"
#include "../../parser/Program.hpp"
#include "../Module.hpp"

namespace ls {

class LSNull;

class NullSTD : public Module {
public:
	NullSTD();
};

}

#endif
