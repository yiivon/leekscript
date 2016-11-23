#include "ValueSTD.hpp"
#include "../value/LSBoolean.hpp"
#include "../value/LSString.hpp"

namespace ls {

ValueSTD::ValueSTD() : Module("Value") {

	operator_("instanceof", {
		{Type::POINTER, Type::CLASS, Type::BOOLEAN, (void*) &ValueSTD::instanceof_ptr}
	});
}

bool ValueSTD::instanceof_ptr(LSValue* x, LSClass* c) {
	bool r = ((LSClass*) x->getClass())->name == c->name;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(c);
	return r;
}

}
