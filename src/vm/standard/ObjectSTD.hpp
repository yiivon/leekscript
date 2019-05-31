#ifndef OBJECTSTD_HPP
#define OBJECTSTD_HPP

#include "../Module.hpp"

namespace ls {

class LSObject;

class ObjectSTD : public Module {
public:
	ObjectSTD(VM* vm);

	static LSObject* readonly;
	static LSNumber* readonly_value;

	static Compiler::value in_any(Compiler& c, std::vector<Compiler::value> args, bool);
	static LSValue* object_new(LSClass* clazz);
};

}

#endif
