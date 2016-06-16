#include "BooleanSTD.hpp"

namespace ls {

BooleanSTD::BooleanSTD() : Module("Boolean") {

	static_method("compare", Type::INTEGER, {Type::BOOLEAN, Type::BOOLEAN}, (void*) &boolean_compare);
}

int boolean_compare(bool a, bool b) {
	if (a) {
		if (!b) return 1;
	} else {
		if (b) return -1;
	}
	return 0;
}

}
