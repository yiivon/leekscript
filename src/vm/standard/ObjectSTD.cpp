#include "ObjectSTD.hpp"
#include "../value/LSObject.hpp"

namespace ls {

ObjectSTD::ObjectSTD() : Module("Object") {

	method("map", Type::OBJECT, Type::OBJECT, {}, (void*) &object_map);

	method("keys", Type::OBJECT, Type::STRING_ARRAY, {}, (void*) &LSObject::get_keys);
	method("values", Type::OBJECT, Type::ARRAY, {}, (void*) &LSObject::get_values);
}

LSObject* object_map(const LSObject*) {

	LSObject* new_obj = new LSObject();

	return new_obj;
}

}
