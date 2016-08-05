#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"

using namespace std;

namespace ls {

LSValue* LSNull::null_var = new LSNull();
LSClass* LSNull::null_class = new LSClass("Null");

LSValue* LSNull::get() {
	return null_var;
}

LSNull::LSNull() {
	refs = 1;
	native = true; // We create one null value which is destroyed only at the end
}

LSNull::~LSNull() {
	cout << "delete LSNull" << endl;
}

bool LSNull::isTrue() const {
	return false;
}

LSValue* LSNull::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSNull::operator += (LSValue* value) {
	return value->operator += (this);
}
LSValue* LSNull::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSNull::operator -= (LSValue* value) {
	return value->operator -= (this);
}
LSValue* LSNull::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSNull::operator *= (LSValue* value) {
	return value->operator *= (this);
}
LSValue* LSNull::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSNull::operator /= (LSValue* value) {
	return value->operator /= (this);
}
LSValue* LSNull::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSNull::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
LSValue* LSNull::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSNull::operator %= (LSValue* value) {
	return value->operator %= (this);
}

bool LSNull::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSNull::operator == (const LSNull*) const {
	return true;
}

bool LSNull::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSNull::operator < (const LSNull*) const {
	return false;
}
bool LSNull::operator < (const LSBoolean*) const {
	return true;
}
bool LSNull::operator < (const LSNumber*) const {
	return true;
}
bool LSNull::operator < (const LSString*) const {
	return true;
}
bool LSNull::operator < (const LSArray<LSValue*>*) const {
	return true;
}
bool LSNull::operator < (const LSArray<int>*) const {
	return true;
}
bool LSNull::operator < (const LSArray<double>*) const {
	return true;
}
bool LSNull::operator < (const LSFunction*) const {
	return true;
}
bool LSNull::operator < (const LSObject*) const {
	return true;
}
bool LSNull::operator < (const LSClass*) const {
	return true;
}

bool LSNull::operator > (const LSValue* v) const {
	return v->operator > (this);
}
bool LSNull::operator > (const LSNull*) const {
	return false;
}
bool LSNull::operator > (const LSBoolean*) const {
	return false;
}
bool LSNull::operator > (const LSNumber*) const {
	return false;
}
bool LSNull::operator > (const LSString*) const {
	return false;
}
bool LSNull::operator > (const LSArray<LSValue*>*) const {
	return false;
}
bool LSNull::operator > (const LSArray<int>*) const {
	return false;
}
bool LSNull::operator > (const LSArray<double>*) const {
	return false;
}
bool LSNull::operator > (const LSFunction*) const {
	return false;
}
bool LSNull::operator > (const LSObject*) const {
	return false;
}
bool LSNull::operator > (const LSClass*) const {
	return false;
}

bool LSNull::operator <= (const LSValue* v) const {
	return v->operator <= (this);
}
bool LSNull::operator <= (const LSNull*) const {
	return true;
}
bool LSNull::operator <= (const LSBoolean*) const {
	return true;
}
bool LSNull::operator <= (const LSNumber*) const {
	return true;
}
bool LSNull::operator <= (const LSString*) const {
	return true;
}
bool LSNull::operator <= (const LSArray<LSValue*>*) const {
	return true;
}
bool LSNull::operator <= (const LSArray<int>*) const {
	return true;
}
bool LSNull::operator <= (const LSArray<double>*) const {
	return true;
}
bool LSNull::operator <= (const LSFunction*) const {
	return true;
}
bool LSNull::operator <= (const LSObject*) const {
	return true;
}
bool LSNull::operator <= (const LSClass*) const {
	return true;
}

bool LSNull::operator >= (const LSValue* v) const {
	return v->operator >= (this);
}
bool LSNull::operator >= (const LSNull*) const {
	return true;
}
bool LSNull::operator >= (const LSBoolean*) const {
	return false;
}
bool LSNull::operator >= (const LSNumber*) const {
	return false;
}
bool LSNull::operator >= (const LSString*) const {
	return false;
}
bool LSNull::operator >= (const LSArray<LSValue*>*) const {
	return false;
}
bool LSNull::operator >= (const LSArray<int>*) const {
	return false;
}
bool LSNull::operator >= (const LSArray<double>*) const {
	return false;
}
bool LSNull::operator >= (const LSFunction*) const {
	return false;
}
bool LSNull::operator >= (const LSObject*) const {
	return false;
}
bool LSNull::operator >= (const LSClass*) const {
	return false;
}

LSValue* LSNull::at(const LSValue*) const {
	return LSNull::get();
}
LSValue** LSNull::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSNull::range(int, int) const {
	return this->clone();
}
LSValue* LSNull::rangeL(int, int) {
	return this;
}

LSValue* LSNull::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}
LSValue** LSNull::attrL(const LSValue*) {
	return nullptr;
}

LSValue* LSNull::clone() const {
	return LSNull::get();
}

std::ostream& LSNull::print(std::ostream& os) const {
	os << "null";
	return os;
}
string LSNull::json() const {
	return "\"1|null\"";
}

LSValue* LSNull::getClass() const {
	return LSNull::null_class;
}

int LSNull::typeID() const {
	return 1;
}

const BaseRawType* LSNull::getRawType() const {
	return RawType::NULLL;
}

}
