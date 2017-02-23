#include <exception>

#include "LSInterval.hpp"
#include "LSNumber.hpp"
#include "LSNull.hpp"

using namespace std;

namespace ls {

LSValue* LSInterval::clazz;

LSInterval::LSInterval() : LSValue(INTERVAL) {}

LSInterval::~LSInterval() {}

LSArray<int>* LSInterval::ls_filter(LSFunction<bool>* function) {
	auto fun = (bool (*)(void*, int)) function->function;

	LSArray<int>* new_array = new LSArray<int>();

	int i = this->a;
	while (i <= b) {
		if (fun(function, i)) new_array->push_clone(i);
		i++;
	}
	if (this->refs == 0) delete this;
	return new_array;
}

bool LSInterval::isTrue() const {
	return b >= a;
}

bool LSInterval::in_v(int value) const {
	bool res = value >= a and value <= b;
	LSValue::delete_temporary(this);
	return res;
}

int LSInterval::atv(const int key) const {
	int size = b - a + 1;
	if (key < 0 or key >= size) {
		LSValue::delete_temporary(this);
		jit_exception_throw(new VM::ExceptionObj(VM::Exception::ARRAY_OUT_OF_BOUNDS));
	}
	return a + key;
}

LSValue* LSInterval::at(const LSValue* key) const {
	if (key->type != NUMBER) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(key);
		jit_exception_throw(new VM::ExceptionObj(VM::Exception::ARRAY_KEY_IS_NOT_NUMBER));
	}
	auto n = static_cast<const LSNumber*>(key);
	int size = b - a + 1;
	int index = n->value;
	if (index < 0 or index >= size) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(key);
		jit_exception_throw(new VM::ExceptionObj(VM::Exception::ARRAY_OUT_OF_BOUNDS));
	}
	return LSNumber::get(a + index);
}

LSValue* LSInterval::range(int start, int end) const {
	int size = b - a + 1;
	if (start < 0 or start >= size or end >= b) {
		LSValue::delete_temporary(this);
		jit_exception_throw(new VM::ExceptionObj(VM::Exception::ARRAY_OUT_OF_BOUNDS));
	}
	auto new_interval = new LSInterval();
	new_interval->a = a + start;
	new_interval->b = a + end;
	return new_interval;
}

int LSInterval::abso() const {
	return b - a + 1;
}

LSValue* LSInterval::clone() const {
	LSInterval* new_interval = new LSInterval();
	new_interval->a = a;
	new_interval->b = b;
	return new_interval;
}

std::ostream& LSInterval::dump(std::ostream& os) const {
	if (a == b) {
		os << "[" << a << "]";
	} else {
		os << "[" << a << ".." << b << "]";
	}
	return os;
}

LSValue* LSInterval::getClass() const {
	return LSInterval::clazz;
}

}
