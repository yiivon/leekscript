#include "ObjectSTD.hpp"

ObjectSTD::ObjectSTD() : Module("Object") {

	method("map", Type::OBJECT, {Type::OBJECT}, (void*) &object_map);
}

LSObject* object_map(const LSObject*) {

	LSObject* new_obj = new LSObject();

	return new_obj;
}
