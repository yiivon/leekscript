#ifndef SYSTEMSTD_HPP
#define SYSTEMSTD_HPP

#include "../VM.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class SystemSTD : public Module {
public:
	SystemSTD();
};

}

#endif
