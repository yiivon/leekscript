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
};

LSObject* object_map(const LSObject* object);

}

#endif
