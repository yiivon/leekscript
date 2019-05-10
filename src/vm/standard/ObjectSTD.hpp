#ifndef OBJECTSTD_HPP
#define OBJECTSTD_HPP

#include "../Module.hpp"

namespace ls {

class LSObject;

class ObjectSTD : public Module {
public:
	ObjectSTD();

	static LSObject readonly;
	static LSNumber* readonly_value;

	static Compiler::value in_any(Compiler& c, std::vector<Compiler::value> args, bool);
	static LSValue* object_new(LSClass* clazz);
	static void set_field_default_value(LSClass* clazz, char* field_name, LSValue* default_value);
};

}

#endif
