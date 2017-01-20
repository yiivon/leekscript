#include <iostream>
#include "LSValue.hpp"
#include "value/LSNumber.hpp"
#include "value/LSNull.hpp"
#include "value/LSBoolean.hpp"
#include "value/LSArray.hpp"
#include "value/LSMap.hpp"
#include "value/LSSet.hpp"
#include "value/LSObject.hpp"
#include "value/LSFunction.hpp"
#include "VM.hpp"

using namespace std;

namespace ls {

int LSValue::obj_count = 0;
int LSValue::obj_deleted = 0;

LSValue::LSValue() : refs(0) {
	obj_count++;
	#if DEBUG_LEAKS_DETAILS
		objs().insert({this, this});
	#endif
}

LSValue::LSValue(const LSValue& ) : refs(0) {
	obj_count++;
	#if DEBUG_LEAKS_DETAILS
		objs().insert({this, this});
	#endif
}

template <class T>
LSValue* LSValue::get(T v) {
	return LSNull::get();
}

template <>
LSValue* LSValue::get(int v) {
	return LSNumber::get(v);
}
template <>
LSValue* LSValue::get(double v) {
	return LSNumber::get(v);
}

LSValue::~LSValue() {
	obj_deleted++;
	#if DEBUG_LEAKS_DETAILS
		objs().erase(this);
	#endif
}

LSValue* LSValue::ls_minus()                                 { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_not()                                   { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_tilde()                                 { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_preinc()                                { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_inc()                                   { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_predec()                                { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_dec()                                   { delete_temporary(this); return LSNull::get(); }

LSValue* LSValue::add(LSValue* v) {
	delete_temporary(this);
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::add_eq(LSValue* v) {
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::sub(LSValue* v) {
	delete_temporary(this);
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::sub_eq(LSValue* v) {
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::mul(LSValue* v) {
	delete_temporary(this);
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::mul_eq(LSValue* v) {
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::div(LSValue* v) {
	delete_temporary(this);
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::div_eq(LSValue* v) {
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::int_div(LSValue* v) {
	delete_temporary(this);
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::int_div_eq(LSValue* v) {
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::ls_pow(LSNull*)                            { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSBoolean*)                         { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSNumber* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSString* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSArray<LSValue*>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSArray<int>* value)                { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSArray<double>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<LSValue*,LSValue*>* value)    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<LSValue*,int>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<LSValue*,double>* value)      { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<int,LSValue*>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<int,int>* value)              { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<int,double>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSSet<LSValue*>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSSet<int>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSSet<double>* value)               { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSObject* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSFunction<LSValue*>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSClass* value)                     { delete_temporary(this); delete_temporary(value); return LSNull::get(); }

LSValue* LSValue::ls_pow_eq(LSNull*)                         { return this; }
LSValue* LSValue::ls_pow_eq(LSBoolean*)                      { return this; }
LSValue* LSValue::ls_pow_eq(LSNumber* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSString* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSArray<LSValue*>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSArray<int>* value)             { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSArray<double>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<LSValue*,LSValue*>* value) { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<LSValue*,int>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<LSValue*,double>* value)   { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<int,LSValue*>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<int,int>* value)           { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<int,double>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSSet<LSValue*>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSSet<int>* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSSet<double>* value)            { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSObject* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSFunction<LSValue*>* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSClass* value)                  { delete_temporary(value); return this; }

LSValue* LSValue::ls_mod(LSNull*)                            { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSBoolean*)                         { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSNumber* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSString* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSArray<LSValue*>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSArray<int>* value)                { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSArray<double>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<LSValue*,LSValue*>* value)    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<LSValue*,int>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<LSValue*,double>* value)      { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<int,LSValue*>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<int,int>* value)              { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<int,double>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSSet<LSValue*>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSSet<int>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSSet<double>* value)               { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSObject* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSFunction<LSValue*>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSClass* value)                     { delete_temporary(this); delete_temporary(value); return LSNull::get(); }

LSValue* LSValue::ls_mod_eq(LSNull*)                         { return this; }
LSValue* LSValue::ls_mod_eq(LSBoolean*)                      { return this; }
LSValue* LSValue::ls_mod_eq(LSNumber* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSString* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSArray<LSValue*>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSArray<int>* value)             { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSArray<double>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<LSValue*,LSValue*>* value) { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<LSValue*,int>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<LSValue*,double>* value)   { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<int,LSValue*>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<int,int>* value)           { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<int,double>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSSet<LSValue*>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSSet<int>* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSSet<double>* value)            { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSObject* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSFunction<LSValue*>* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSClass* value)                  { delete_temporary(value); return this; }

bool LSValue::eq(const LSNull*) const                        { return false; }
bool LSValue::eq(const LSBoolean*) const                     { return false; }
bool LSValue::eq(const LSNumber*) const                      { return false; }
bool LSValue::eq(const LSString*) const                      { return false; }
bool LSValue::eq(const LSArray<LSValue*>*) const             { return false; }
bool LSValue::eq(const LSArray<int>*) const                  { return false; }
bool LSValue::eq(const LSArray<double>*) const               { return false; }
bool LSValue::eq(const LSMap<LSValue*,LSValue*>*) const      { return false; }
bool LSValue::eq(const LSMap<LSValue*,int>*) const           { return false; }
bool LSValue::eq(const LSMap<LSValue*,double>*) const        { return false; }
bool LSValue::eq(const LSMap<int,LSValue*>*) const           { return false; }
bool LSValue::eq(const LSMap<int,int>*) const                { return false; }
bool LSValue::eq(const LSMap<int,double>*) const             { return false; }
bool LSValue::eq(const LSMap<double,LSValue*>*) const        { return false; }
bool LSValue::eq(const LSMap<double,int>*) const             { return false; }
bool LSValue::eq(const LSMap<double,double>*) const          { return false; }
bool LSValue::eq(const LSSet<LSValue*>*) const               { return false; }
bool LSValue::eq(const LSSet<int>*) const                    { return false; }
bool LSValue::eq(const LSSet<double>*) const                 { return false; }
bool LSValue::eq(const LSFunction<LSValue*>*) const                    { return false; }
bool LSValue::eq(const LSObject*) const                      { return false; }
bool LSValue::eq(const LSClass*) const                       { return false; }

bool LSValue::lt(const LSNull*) const                        { return typeID() < 1; }
bool LSValue::lt(const LSBoolean*) const                     { return typeID() < 2; }
bool LSValue::lt(const LSNumber*) const                      { return typeID() < 3; }
bool LSValue::lt(const LSString*) const                      { return typeID() < 4; }
bool LSValue::lt(const LSArray<LSValue*>*) const             { return typeID() < 5; }
bool LSValue::lt(const LSArray<int>*) const                  { return typeID() < 5; }
bool LSValue::lt(const LSArray<double>*) const               { return typeID() < 5; }
bool LSValue::lt(const LSMap<LSValue*,LSValue*>*) const      { return typeID() < 6; }
bool LSValue::lt(const LSMap<LSValue*,int>*) const           { return typeID() < 6; }
bool LSValue::lt(const LSMap<LSValue*,double>*) const        { return typeID() < 6; }
bool LSValue::lt(const LSMap<int,LSValue*>*) const           { return typeID() < 6; }
bool LSValue::lt(const LSMap<int,int>*) const                { return typeID() < 6; }
bool LSValue::lt(const LSMap<int,double>*) const             { return typeID() < 6; }
bool LSValue::lt(const LSMap<double,LSValue*>*) const        { return typeID() < 6; }
bool LSValue::lt(const LSMap<double,int>*) const             { return typeID() < 6; }
bool LSValue::lt(const LSMap<double,double>*) const          { return typeID() < 6; }
bool LSValue::lt(const LSSet<LSValue*>*) const               { return typeID() < 7; }
bool LSValue::lt(const LSSet<int>*) const                    { return typeID() < 7; }
bool LSValue::lt(const LSSet<double>*) const                 { return typeID() < 7; }
bool LSValue::lt(const LSFunction<LSValue*>*) const                    { return typeID() < 8; }
bool LSValue::lt(const LSObject*) const                      { return typeID() < 9; }
bool LSValue::lt(const LSClass*) const                       { return typeID() < 10; }

LSValue* LSValue::at(const LSValue*) const {
	return LSNull::get();
}

LSValue** LSValue::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSValue::attr(const std::string& key) const {
	if (key == "class") {
		return getClass();
	}
	auto method = ((LSClass*) getClass())->getDefaultMethod(key);
	if (method != nullptr) {
		//std::cout << "method " << key << " of class " << getClass() << std::endl;
		return method;
	}
	return LSNull::get();
}

LSValue** LSValue::attrL(const std::string&) {
	return nullptr;
}

LSValue* LSValue::range(int, int) const {
	return clone();
}

LSValue* LSValue::rangeL(int, int) {
	return this;
}

LSValue* LSValue::abso() const {
	return LSNull::get();
}

LSValue* LSValue::clone() const {
	return (LSValue*) this;
}

//std::ostream& operator << (std::ostream& os, LSValue& value) {
//	value.print(os);
//	return os;
//}

bool LSValue::isInteger() const {
	if (const LSNumber* v = dynamic_cast<const LSNumber*>(this)) {
		return v->isInteger();
	}
	return false;
}

LSValue* LSValue::get_from_json(Json& json) {
	switch (json.type()) {
		case Json::value_t::null:
			return LSNull::get();
		case Json::value_t::boolean:
			return LSBoolean::get(json);
		case Json::value_t::number_integer:
		case Json::value_t::number_unsigned:
		case Json::value_t::number_float:
			return LSNumber::get(json);
		case Json::value_t::string:
			return new LSString(json);
		case Json::value_t::array: {
			auto array = new LSArray<LSValue*>();
			for (auto& v : json) {
				array->push_move(get_from_json(v));
			}
			return array;
		}
		case Json::value_t::object: {
			auto object = new LSObject();
			for (Json::iterator it = json.begin(); it != json.end(); ++it) {
				object->addField(it.key(), get_from_json(it.value()));
			}
			return object;
		}
		case Json::value_t::discarded:
			return LSNull::get();
	}
	throw std::exception();
}

std::string LSValue::to_string() const {
	std::ostringstream oss;
	print(oss);
	return oss.str();
}

LSString* LSValue::ls_json() {
	LSString* json = new LSString(this->json());
	LSValue::delete_temporary(this);
	return json;
}

std::ostream& LSValue::print(std::ostream& os) const {
	dump(os);
	return os;
}

std::string LSValue::json() const {
	std::ostringstream oss;
	print(oss);
	return oss.str();
}

}

namespace std {
	std::string to_string(ls::LSValue* value) {
		std::ostringstream oss;
		value->dump(oss);
		return oss.str();
	}
	std::ostream& operator << (std::ostream& os, const ls::LSValue* value) {
		value->dump(os);
		return os;
	}
}
