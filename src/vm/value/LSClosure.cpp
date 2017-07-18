#include "LSClosure.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"
#include "../LSValue.hpp"

namespace ls {

LSClosure::LSClosure(void* function) : LSFunction(function) {
	type = CLOSURE;
}

LSClosure::~LSClosure() {
	for (size_t i = 0; i < captures.size(); ++i) {
		if (!captures_native[i] and captures[i] != this)
			LSValue::delete_ref(captures[i]);
	}
}

bool LSClosure::closure() const {
	return true;
}

void LSClosure::add_capture(LSValue* value) {
	if (!value->native && value != this) {
		value->refs++;
	}
	captures.push_back(value);
	captures_native.push_back(value->native);
}

LSValue* LSClosure::get_capture(int index) {
	return captures[index];
}

std::ostream& LSClosure::dump(std::ostream& os, int) const {
	os << "<closure>";
	return os;
}

}
