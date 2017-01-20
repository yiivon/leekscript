#include <cmath>
#include <sstream>

#include "LSNumber.hpp"
#include "LSNull.hpp"
#include "LSFunction.hpp"
#include "LSBoolean.hpp"

using namespace std;

namespace ls {

LSClass* LSNumber::number_class = new LSClass("Number");

LSNumber* LSNumber::get(NUMBER_TYPE i) {
	return new LSNumber(i);
}

std::string LSNumber::print(double d) {
	std::string s;
	size_t len = snprintf(0, 0, "%.10f", d);
	size_t oldsize = s.size();
	s.resize(oldsize + len + 1);

	// technically non-portable
	snprintf(&s[oldsize], len+1, "%.10f", d);
	// remove nul terminator
	s.pop_back();
	// remove trailing zeros
	s.erase(s.find_last_not_of('0') + 1, string::npos);
	// remove trailing point
	if (s.back() == L'.') {
		s.pop_back();
	}
	return s;
}

LSNumber::LSNumber() : value(0) {}

LSNumber::LSNumber(NUMBER_TYPE value) : value(value) {}

LSNumber::LSNumber(Json& json) : value(json) {}

LSNumber::~LSNumber() {}

/*
 * LSNumber methods
 */
LSValue* LSNumber::ls_fold(LSFunction<LSValue*>* function, LSValue* v0) {
	auto fun = (LSValue* (*)(void*, LSValue*, int)) function->function;
	LSValue* result = ls::move(v0);
	int number = this->value;
	while (number) {
        int digit = number % 10;
		result = fun(function, result, digit);
        number /= 10;
    }
	LSValue::delete_temporary(this);
	return result;
}

/*
 * LSValue methods
 */
bool LSNumber::isTrue() const {
	return value != 0;
}

LSValue* LSNumber::ls_minus() {
	if (refs == 0) {
		value = -value;
		return this;
	}
	return LSNumber::get(-value);
}

LSValue* LSNumber::ls_not() {
	bool r = value == 0;
	if (refs == 0) delete this;
	return LSBoolean::get(r);
}

LSValue* LSNumber::ls_tilde() {
	if (refs == 0) {
		value = ~ (int)value;
		return this;
	}
	return LSNumber::get(~ (int)value);
}

LSValue* LSNumber::ls_preinc() {
	// ++x
	value += 1;
	return this;
}

LSValue* LSNumber::ls_inc() {
	// x++
	if (refs == 0) {
		value += 1;
		return this;
	}
	LSValue* r = LSNumber::get(value);
	value += 1;
	return r;
}

LSValue* LSNumber::ls_predec() {
	value -= 1;
	return this;
}

LSValue* LSNumber::ls_dec() {
	if (refs == 0) {
		value -= 1;
		return this;
	}
	LSValue* r = LSNumber::get(value);
	value -= 1;
	return r;
}

LSValue* LSNumber::add(LSValue* v) {
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (refs == 0) {
			value += number->value;
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			number->value += value;
			return number;
		}
		return LSNumber::get(this->value + number->value);
	}
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		if (boolean->value) {
			if (refs == 0) {
				this->value += 1;
				return this;
			}
			return LSNumber::get(value + 1);
		}
		return this;
	}
	if (auto string = dynamic_cast<LSString*>(v)) {
		LSValue* r = new LSString(toString() + *string);
		LSValue::delete_temporary(this);
		if (string->refs == 0) delete string;
		return r;
	}
	LSValue::delete_temporary(this);
	return LSNull::get();
}

LSValue* LSNumber::add_eq(LSValue* v) {
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		value += boolean->value;
		return this;
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		value += number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	return LSNull::get();
}

LSValue* LSNumber::sub(LSValue* v) {
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (refs == 0) {
			value -= number->value;
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			number->value = this->value - number->value;
			return number;
		}
		return LSNumber::get(this->value - number->value);
	}
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		if (boolean->value) {
			if (refs == 0) {
				this->value -= 1;
				return this;
			}
			return LSNumber::get(value - 1);
		}
		return this;
	}
	LSValue::delete_temporary(this);
	return LSNull::get();
}

LSValue* LSNumber::sub_eq(LSValue* v) {
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		value -= number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		value -= boolean->value;
	}
	return this;
}

LSValue* LSNumber::mul(LSValue* v) {
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		if (boolean->value) {
			return this;
		}
		if (refs == 0) {
			value = 0;
			return this;
		}
		return LSNumber::get(0);
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (refs == 0) {
			value *= number->value;
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			number->value *= value;
			return number;
		}
		return LSNumber::get(value * number->value);
	}
	if (auto string = dynamic_cast<LSString*>(v)) {
		std::string r;
		for (int i = 0; i < value; ++i) {
			r += *string;
		}
		if (refs == 0) delete this;
		if (string->refs == 0) {
			*string = r;
			return string;
		}
		return new LSString(r);
	}
	LSValue::delete_temporary(this);
	return LSNull::get();
}

LSValue* LSNumber::mul_eq(LSValue* v) {
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		value *= boolean->value;
		return this;
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		value *= number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	return LSNull::get();
}

LSValue* LSNumber::div(LSValue* v) {
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		if (boolean->value) {
			return this;
		}
		if (refs == 0) {
			value = NAN;
			return this;
		}
		return LSNumber::get(NAN);
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (refs == 0) {
			value /= number->value;
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			number->value = value / number->value;
			return number;
		}
		return LSNumber::get(value / number->value);
	}
	LSValue::delete_temporary(this);
	return LSNull::get();
}

LSValue* LSNumber::div_eq(LSValue* v) {
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		if (!boolean->value) {
			value = NAN;
		}
		return this;
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		value /= number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	return LSNull::get();
}

LSValue* LSNumber::int_div(LSValue* v) {
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		if (boolean->value) {
			return this;
		}
		if (refs == 0) {
			value = NAN;
			return this;
		}
		return LSNumber::get(NAN);
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (refs == 0) {
			value /= number->value;
			value = floor(value);
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			number->value = floor(value / number->value);
			return number;
		}
		return LSNumber::get(floor(value / number->value));
	}
	LSValue::delete_temporary(this);
	return LSNull::get();
}

LSValue* LSNumber::int_div_eq(LSValue* v) {
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		if (!boolean->value) {
			value = NAN;
		}
		value = floor(value);
		return this;
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		value /= number->value;
		value = floor(value);
		if (number->refs == 0) delete number;
		return this;
	}
	return LSNull::get();
}

LSValue* LSNumber::pow(LSValue* v) {
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		if (boolean->value) {
			return this;
		}
		if (refs == 0) {
			value = 1;
			return this;
		}
		return LSNumber::get(1);
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (refs == 0) {
			value = std::pow(value, number->value);
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			number->value = std::pow(value, number->value);
			return number;
		}
		return LSNumber::get(std::pow(value, number->value));
	}
	LSValue::delete_temporary(this);
	return LSNull::get();
}

LSValue* LSNumber::pow_eq(LSValue* v) {
	if (auto boolean = dynamic_cast<LSBoolean*>(v)) {
		if (!boolean->value) value = 1;
		return this;
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		value = std::pow(value, number->value);
		if (number->refs == 0) delete number;
		return this;
	}
	return LSNull::get();
}

LSValue* LSNumber::mod(LSValue* v) {
	if (dynamic_cast<LSBoolean*>(v)) {
		if (refs == 0) {
			value = 0;
			return this;
		}
		return LSNumber::get(0);
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (refs == 0) {
			value = fmod(value, number->value);
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			number->value = fmod(value, number->value);
			return number;
		}
		return LSNumber::get(fmod(value, number->value));
	}
	LSValue::delete_temporary(this);
	return LSNull::get();
}

LSValue* LSNumber::mod_eq(LSValue* v) {
	if (dynamic_cast<LSBoolean*>(v)) {
		value = 0;
		return this;
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		value = fmod(value, number->value);
		if (number->refs == 0) delete number;
		return this;
	}
	return LSNull::get();
}

bool LSNumber::operator == (int v) const {
	return value == v;
}

bool LSNumber::operator == (double v) const {
	return value == v;
}

bool LSNumber::operator < (int v) const {
	return value < v;
}

bool LSNumber::operator < (double v) const {
	return value < v;
}

bool LSNumber::eq(const LSValue* v) const {
	if (auto number = dynamic_cast<const LSNumber*>(v)) {
		return this->value == number->value;
	}
	return false;
}

bool LSNumber::lt(const LSValue* v) const {
	if (auto number = dynamic_cast<const LSNumber*>(v)) {
		return this->value < number->value;
	}
	return LSValue::lt(v);
}

LSValue* LSNumber::at(const LSValue*) const {
	return LSNull::get();
}

LSValue** LSNumber::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSNumber::abso() const {
	return LSNumber::get(abs((int) value));
}

int LSNumber::integer() const {
	return value;
}

double LSNumber::real() const {
	return value;
}

LSValue* LSNumber::clone() const {
	return LSNumber::get(this->value);
}

bool LSNumber::isInteger() const {
	return value == (int)value;
}

string LSNumber::toString() const {

	if (isInteger()) return std::to_string((int)value);

	return LSNumber::print(value);
}
string LSNumber::json() const {
	return toString();
}

std::ostream& LSNumber::dump(std::ostream& os) const {
	os << toString();
	return os;
}

LSValue* LSNumber::getClass() const {
	return LSNumber::number_class;
}

int LSNumber::typeID() const {
	return 3;
}

}
