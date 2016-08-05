#include "LSClass.hpp"
#include "LSNull.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "LSFunction.hpp"
#include "../Module.hpp"

using namespace std;

namespace ls {

LSValue* LSClass::class_class(new LSClass("Class", 1));

LSClass::LSClass() : name("?") {
	parent = nullptr;
}

LSClass::LSClass(string name) : name(name) {
	parent = nullptr;
}

LSClass::LSClass(string name, int refs) : name(name) {
	parent = nullptr;
	this->refs = refs;
}

LSClass::LSClass(Json&) {
	parent = nullptr;
	// TODO
}

LSClass::~LSClass() {
	for (auto s : static_fields) {
		if (s.second.value != nullptr) {
			LSValue::delete_val(s.second.value);
		}
	}
}

void LSClass::addMethod(string& name, vector<Method>& method) {
	methods.insert({name, method});
}

void LSClass::addStaticMethod(string& name, vector<StaticMethod>& method) {
	static_methods.insert({name, method});

	// Add first implementation as default method
	LSFunction* fun = new LSFunction(method[0].addr);
	static_fields.insert({name, ModuleStaticField(name, Type::FUNCTION_P, fun)});
}

void LSClass::addField(std::string name, Type type) {
	fields.insert({name, type});
}

void LSClass::addStaticField(ModuleStaticField& f) {
	static_fields.insert({f.name, f});
}

void LSClass::addStaticField(std::string name, Type type, LSValue* value) {
	static_fields.insert({name, ModuleStaticField(name, type, value)});
}

Method* LSClass::getMethod(std::string& name, Type obj_type, vector<Type>& args) {
	try {
		vector<Method>& impl = methods.at(name);
		Method* best = nullptr;
		for (auto& m : impl) {
//			cout << "Test impl : " << m.type << endl;

			if (m.obj_type.compatible(obj_type) and Type::list_compatible(m.type.arguments_types, args)) {
//				cout << "Impl good : " << m.type << endl;
				if (best == nullptr or
					Type::list_more_specific(best->type.arguments_types, m.type.arguments_types) or
					Type::more_specific(m.obj_type, best->obj_type)) {
					best = &m;
//					cout << "best : " << m.type << endl;
				}
			}
		}
		return best;
	} catch (exception& e) {
		return nullptr;
	}
}

StaticMethod* LSClass::getStaticMethod(std::string& name, vector<Type>& args) {
	try {
		vector<StaticMethod>& impl = static_methods.at(name);
		StaticMethod* best = nullptr;
		for (auto& m : impl) {
//			cout << "Test impl : " << m.type << endl;
			if (Type::list_compatible(m.type.arguments_types, args)) {
//				cout << "Impl good : " << m.type << endl;
				if (best == nullptr or Type::list_more_specific(best->type.arguments_types, m.type.arguments_types)) {
//					cout << "Better" << endl;
					best = &m;
				}
			}
		}
//		cout << "BEST : " << best->type << endl;
		return best;
	} catch (exception& e) {
		return nullptr;
	}
}

LSFunction* LSClass::getDefaultMethod(string& name) {
	try {
		vector<Method>& impl = methods.at(name);
		return new LSFunction(impl[0].addr);
	} catch (exception& e) {
		return nullptr;
	}
	return nullptr;
}

bool LSClass::isTrue() const {
	return false;
}

LSValue* LSClass::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSClass::operator += (LSValue* value) {
	return value->operator += (this);
}
LSValue* LSClass::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSClass::operator -= (LSValue* value) {
	return value->operator -= (this);
}
LSValue* LSClass::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSClass::operator *= (LSValue* value) {
	return value->operator *= (this);
}
LSValue* LSClass::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSClass::operator /= (LSValue* value) {
	return value->operator /= (this);
}
LSValue* LSClass::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSClass::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
LSValue* LSClass::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSClass::operator %= (LSValue* value) {
	return value->operator %= (this);
}

bool LSClass::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSClass::operator == (const LSClass*) const {
	return false;
}

bool LSClass::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSClass::operator < (const LSNull*) const {
	return false;
}
bool LSClass::operator < (const LSBoolean*) const {
	return false;
}
bool LSClass::operator < (const LSNumber*) const {
	return false;
}
bool LSClass::operator < (const LSString*) const {
	return false;
}
bool LSClass::operator < (const LSArray<LSValue*>*) const {
	return false;
}
bool LSClass::operator < (const LSArray<int>*) const {
	return false;
}
bool LSClass::operator < (const LSArray<double>*) const {
	return false;
}
bool LSClass::operator < (const LSObject*) const {
	return false;
}
bool LSClass::operator < (const LSFunction*) const {
	return false;
}
bool LSClass::operator < (const LSClass*) const {
	return false;
}



LSValue* LSClass::at(const LSValue*) const {
	return LSNull::null_var;
}

LSValue** LSClass::atL(const LSValue*) {
	return &LSNull::null_var;
}

LSValue* LSClass::range(int, int) const {
	return this->clone();
}

LSValue* LSClass::rangeL(int, int) {
	return this;
}

LSValue* LSClass::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	if (key->operator == (new LSString("name"))) {
		return new LSString(name);
	}
	try {
		return static_fields.at(*((LSString*) key)).value;
	} catch (exception& e) {}
	return LSNull::null_var;
}

LSValue** LSClass::attrL(const LSValue*) {
	return &LSNull::null_var;
}

LSValue* LSClass::clone() const {
	return new LSClass(name);
}

std::ostream& LSClass::print(std::ostream& os) const {
	os << "<class " << name << ">";
	return os;
}

string LSClass::json() const {
	return "class";
}

LSValue* LSClass::getClass() const {
	return LSClass::class_class;
}

int LSClass::typeID() const {
	return 10;
}

const BaseRawType* LSClass::getRawType() const {
	return RawType::CLASS;
}

}
