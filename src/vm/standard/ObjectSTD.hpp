#ifndef OBJECTSTD_HPP
#define OBJECTSTD_HPP

#include "../VM.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class LSObject;

class ObjectSTD : public Module {
public:
	ObjectSTD();

	static LSObject readonly;
	static LSNumber* readonly_value;
};

}

#endif
