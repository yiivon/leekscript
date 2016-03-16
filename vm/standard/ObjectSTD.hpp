#ifndef OBJECTSTD_HPP
#define OBJECTSTD_HPP

#include "../VM.hpp"
#include "../../parser/semantic/SemanticAnalyser.hpp"
#include "../../parser/Program.hpp"
#include "../Module.hpp"

class ObjectSTD : public Module {
public:
	ObjectSTD();
};

LSObject* object_map(const LSObject* object);

#endif
