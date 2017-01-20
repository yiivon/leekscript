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

LSValue* LSValue::pow(LSValue* v) {
	delete_temporary(this);
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::pow_eq(LSValue* v) {
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::mod(LSValue* v) {
	delete_temporary(this);
	delete_temporary(v);
	return LSNull::get();
}

LSValue* LSValue::mod_eq(LSValue* v) {
	delete_temporary(v);
	return LSNull::get();
}

bool LSValue::eq(const LSValue*) const {
	return false;
}

bool LSValue::lt(const LSValue* v) const {
	return typeID() < v->typeID();
}

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
