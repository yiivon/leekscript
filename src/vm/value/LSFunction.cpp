#include "LSFunction.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"

using namespace std;

namespace ls {

LSClass* LSFunction::function_class = new LSClass("Function");

LSFunction::LSFunction(void* function) {
	this->function = function;
	this->refs = 0;
	this->native_function = false;
}

LSFunction::LSFunction(void* function, int refs, bool native) {
	this->function = function;
	this->refs = refs;
	this->native_function = native;
}

LSFunction::LSFunction(Json&) {
	// TODO
}

LSFunction::~LSFunction() {}

bool LSFunction::native() const {
	return native_function;
}

bool LSFunction::isTrue() const {
	return true;
}

LSValue* LSFunction::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSFunction::operator *= (LSValue* value) {
	return value->operator *= (this);
}
LSValue* LSFunction::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSFunction::operator /= (LSValue* value) {
	return value->operator /= (this);
}
LSValue* LSFunction::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSFunction::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
LSValue* LSFunction::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSFunction::operator %= (LSValue* value) {
	return value->operator %= (this);
}

bool LSFunction::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSFunction::operator == (const LSFunction* f) const {
	return function == f->function;
}

bool LSFunction::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSFunction::operator < (const LSNull*) const {
	return false;
}
bool LSFunction::operator < (const LSBoolean*) const {
	return false;
}
bool LSFunction::operator < (const LSNumber*) const {
	return false;
}
bool LSFunction::operator < (const LSString*) const {
	return false;
}
bool LSFunction::operator < (const LSArray<LSValue*>*) const {
	return false;
}
bool LSFunction::operator < (const LSArray<int>*) const {
	return false;
}
bool LSFunction::operator < (const LSArray<double>*) const {
	return false;
}
bool LSFunction::operator < (const LSObject*) const {
	return false;
}
bool LSFunction::operator < (const LSFunction*) const {
	return false;
}
bool LSFunction::operator < (const LSClass*) const {
	return true;
}




LSValue* LSFunction::at(const LSValue*) const {
	return LSNull::get();
}
LSValue** LSFunction::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSFunction::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}
LSValue** LSFunction::attrL(const LSValue*) {
	return nullptr;
}

LSValue* LSFunction::clone() const {
	return (LSValue*) this;
}

std::ostream& LSFunction::print(std::ostream& os) const {
	os << "<function>";
	return os;
}
string LSFunction::json() const {
	return "\"<function>\"";
}

LSValue* LSFunction::getClass() const {
	return LSFunction::function_class;
}

int LSFunction::typeID() const {
	return 8;
}

const BaseRawType* LSFunction::getRawType() const {
	return RawType::FUNCTION;
}

}
