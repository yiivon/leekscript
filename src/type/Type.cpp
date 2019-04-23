#include <sstream>
#include <numeric>
#include <algorithm>
#include <memory>
#include "Type.hpp"
#include "../constants.h"
#include "../colors.h"
#include "../../lib/utf8.h"
#include "Any_type.hpp"
#include "Array_type.hpp"
#include "Set_type.hpp"
#include "Function_type.hpp"
#include "Class_type.hpp"
#include "Object_type.hpp"
#include "Class_type.hpp"
#include "Interval_type.hpp"
#include "String_type.hpp"
#include "Number_type.hpp"
#include "Long_type.hpp"
#include "Bool_type.hpp"
#include "Mpz_type.hpp"
#include "Real_type.hpp"
#include "Map_type.hpp"
#include "Null_type.hpp"
#include "Integer_type.hpp"
#include "I8_type.hpp"
#include "Placeholder_type.hpp"
#include "Struct_type.hpp"
#include "Pointer_type.hpp"
#include "Template_type.hpp"
#include "Never_type.hpp"
#include "../compiler/value/Function.hpp"
#include "../compiler/value/Value.hpp"

namespace ls {

std::shared_ptr<Base_type> Type::_raw_never = nullptr;
std::shared_ptr<Base_type> Type::_raw_null = nullptr;
std::shared_ptr<Base_type> Type::_raw_any = nullptr;
std::shared_ptr<Base_type> Type::_raw_boolean = nullptr;
std::shared_ptr<Base_type> Type::_raw_i8 = nullptr;
std::shared_ptr<Base_type> Type::_raw_integer = nullptr;
std::shared_ptr<Base_type> Type::_raw_number = nullptr;
std::shared_ptr<Base_type> Type::_raw_long = nullptr;
std::shared_ptr<Base_type> Type::_raw_real = nullptr;
std::shared_ptr<Base_type> Type::_raw_mpz = nullptr;
std::shared_ptr<Base_type> Type::_raw_string = nullptr;
std::shared_ptr<Base_type> Type::_raw_interval = nullptr;
std::shared_ptr<Base_type> Type::_raw_object = nullptr;
std::map<std::string, std::shared_ptr<Base_type>> Type::_raw_class;

std::vector<std::shared_ptr<const Base_type>> Type::placeholder_types;

void Type::clear_placeholder_types() {
	placeholder_types.clear();
}

unsigned int Type::placeholder_counter = 0;

Type::Type() {
	native = false;
}
Type::Type(std::initializer_list<std::shared_ptr<const Base_type>> types) {
	_types = types;
}
Type::Type(std::initializer_list<Type> types) {
	for (const auto& t : types) {
		operator += (t);
	}
}
Type::Type(std::shared_ptr<const Base_type> raw_type, bool native, bool temporary, bool constant) {
	_types.push_back(raw_type);
	this->native = native;
	this->temporary = temporary;
	this->constant = constant;
}

int Type::id() const {
	if (_types.size() == 0) { return 0; }
	return fold()._types[0]->id();
}

bool Type::must_manage_memory() const {
	if (_types.size() == 0) { return false; }
	return is_polymorphic() and not native;
}

Type Type::return_type() const {
	if (_types.size() == 0) { return {}; }
	return _types[0]->return_type();
}
const Type Type::argument(size_t index) const {
	if (_types.size() == 0) { return {}; }
	return _types[0]->argument(index);
}
const std::vector<Type> Type::arguments() const {
	if (_types.size() == 0) { return {}; }
	return _types[0]->arguments();
}
const Type Type::element() const {
	Type element;
	for (const auto& type : _types) {
		element += type->element();
	}
	return element;
}
const Type Type::key() const {
	if (_types.size() == 0) { return {}; }
	return _types[0]->key();
}
const Type Type::member(int i) const {
	if (_types.size() == 0) { return {}; }
	return _types[0]->member(i);
}

void Type::operator += (const Type type) {
	for (const auto& t : type._types) {
		operator += (t);
	}
	if (type.temporary) temporary = true;
}
void Type::operator += (std::shared_ptr<const Base_type> type) {
	for (const auto& t : _types) {
		if (type->operator == (t.get()))
			return;
	}
	_types.push_back(type);
}
Type Type::operator * (const Type& t2) const {
	if (_types.size() == 0) {
		return t2;
	}
	if (t2._types.size() == 0) {
		return *this;
	}
	if (*this == t2) {
		return *this;
	}
	if (is_polymorphic() and t2.is_primitive()) {
		return Type::any();
	}
	if (t2.is_polymorphic() and is_primitive()) {
		return Type::any();
	}
	// Temporary, to be removed when compatible() is removed
	if ((is_bool() and t2.is_integer()) or (is_integer() and t2.is_bool())) {
		return any();
	}
	if (t2.compatible(*this)) {
		return t2;
	}
	if (compatible(t2)) {
		return *this;
	}
	if (is_array() and t2.is_array()) {
		if (element().is_polymorphic() and t2.element().is_polymorphic()) {
			return array(any());
		}
	}
	return Type::any();
}

Type Type::fold() const {
	if (_types.size() == 1) return *this;
	return std::accumulate(_types.begin(), _types.end(), Type(), [](const Type& type, std::shared_ptr<const Base_type> t) {
		return type * Type(t);
	});
}

void Type::toJson(std::ostream& os) const {
	os << "{\"type\":\"" << getJsonName() << "\"";

	if (is_function()) {
		os << ",\"args\":[";
		for (unsigned t = 0; t < arguments().size(); ++t) {
			if (t > 0) os << ",";
			argument(t).toJson(os);
		}
		os << "]";
		os << ",\"return\":";
		return_type().toJson(os);
	}
	os << "}";
}
std::string Type::getJsonName() const {
	if (_types.size() == 0) {
		return "?";
	}
	return _types[0]->getJsonName();
}

std::string Type::to_string() const {
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

std::string Type::class_name() const {
	if (_types.size() == 0) { return ""; }
	return fold()._types[0]->clazz();
}

bool Type::iterable() const {
	return std::all_of(begin(_types), end(_types), [](auto& type) {
		return type->iterable();
	});
}

bool Type::is_container() const {
	return std::all_of(begin(_types), end(_types), [](auto& type) {
		return type->is_container();
	});
}
bool Type::can_be_container() const {
	return is_any() or is_placeholder() or std::any_of(begin(_types), end(_types), [](auto& type) {
		return type->is_container();
	});
}

bool Type::operator == (const Type& type) const {
	return _types.size() == type._types.size() && temporary == type.temporary && std::equal(_types.begin(), _types.end(), type._types.begin(), [&](std::shared_ptr<const Base_type> t1, std::shared_ptr<const Base_type> t2) {
		return t1->operator == (t2.get());
	});
}

bool Type::operator < (const Type& type) const {
	return _types < type._types;
}

Type Type::not_temporary() const {
	Type new_type = *this;
	new_type.temporary = false;
	return new_type;
}
Type Type::add_temporary() const {
	Type new_type = *this;
	new_type.temporary = true;
	return new_type;
}

llvm::Type* Type::llvm_type(const Compiler& c) const {
	if (_types.size() == 0) {
		return llvm::Type::getVoidTy(c.getContext());
	}
	return fold()._types[0]->llvm(c);
}

Type Type::add_pointer() const {
	Type new_type = *this;
	return new_type;
}

Type Type::iterator() const {
	if (_types.size() > 0) {
		return fold()._types[0]->iterator();
	}
	assert(false && "No iterator for void");
}
Type Type::pointer() const {
	return { std::make_shared<Pointer_type>(*this) };
}
Type Type::pointed() const {
	assert(is_pointer());
	if (_types.size() > 0) {
		return dynamic_cast<const Pointer_type*>(_types[0].get())->pointed();
	}
	assert(false && "Void type is not pointer");
}

template <class T> bool Type::is_type() const {
	return _types.size() && all([&](std::shared_ptr<const Base_type> type) {
		if (dynamic_cast<const T*>(type.get()) != nullptr) return true;
		if (auto t = dynamic_cast<const Template_type*>(type.get())) {
			return t->_implementation.is_type<T>();
		}
		return false;
	});
}
template <class T> bool Type::can_be_type() const {
	return _types.size() && some([&](std::shared_ptr<const Base_type> type) {
		return dynamic_cast<const T*>(type.get()) != nullptr;
	});
}
bool Type::is_any() const { return is_type<Any_type>(); }
bool Type::is_bool() const { return is_type<Bool_type>(); }
bool Type::can_be_bool() const { return can_be_type<Bool_type>(); }
bool Type::is_number() const { return castable(Type::number(), true); }
bool Type::can_be_number() const {
	return some([&](std::shared_ptr<const Base_type> type) {
		return type->distance(Type::number()._types[0].get()) >= 0;
	});
}
bool Type::can_be_numeric() const {
	return is_any() or can_be_bool() or can_be_number();
}
bool Type::is_real() const { return is_type<Real_type>(); }
bool Type::is_integer() const { return is_type<Integer_type>(); }
bool Type::is_long() const { return is_type<Long_type>(); }
bool Type::is_mpz() const { return is_type<Mpz_type>(); }
bool Type::is_string() const { return is_type<String_type>(); }
bool Type::is_array() const { return is_type<Array_type>(); }
bool Type::is_set() const { return is_type<Set_type>(); }
bool Type::is_interval() const { return is_type<Interval_type>(); }
bool Type::is_map() const { return is_type<Map_type>(); }
bool Type::is_function() const { return is_type<Function_type>(); }
bool Type::is_object() const { return is_type<Object_type>(); }
bool Type::is_never() const { return is_type<Never_type>(); }
bool Type::is_null() const { return is_type<Null_type>(); }
bool Type::is_class() const { return is_type<Class_type>(); }
bool Type::is_placeholder() const { return is_type<Placeholder_type>(); }
bool Type::is_pointer() const { return is_type<Pointer_type>(); }
bool Type::is_struct() const { return is_type<Struct_type>(); }
bool Type::is_closure() const {
	return _types.size() && all([&](std::shared_ptr<const Base_type> type) {
		auto f = dynamic_cast<const Function_type*>(type.get());
		return f && f->closure();
	});
}
bool Type::is_polymorphic() const {
	return _types.size() && some([&](std::shared_ptr<const Base_type> t) {
		return dynamic_cast<const Integer_type*>(t.get()) == nullptr
			and dynamic_cast<const Mpz_type*>(t.get()) == nullptr 
			and dynamic_cast<const Long_type*>(t.get()) == nullptr 
			and dynamic_cast<const Real_type*>(t.get()) == nullptr 
			and dynamic_cast<const Bool_type*>(t.get()) == nullptr;
	});
}
bool Type::is_primitive() const {
	return _types.size() && all([&](std::shared_ptr<const Base_type> t) {
		return dynamic_cast<const Integer_type*>(t.get()) != nullptr
			or dynamic_cast<const Mpz_type*>(t.get()) != nullptr 
			or dynamic_cast<const Long_type*>(t.get()) != nullptr 
			or dynamic_cast<const Real_type*>(t.get()) != nullptr 
			or dynamic_cast<const Bool_type*>(t.get()) != nullptr;
	});
}
bool Type::is_callable() const {
	return _types.size() && all([&](std::shared_ptr<const Base_type> t) {
		return t->callable();
	});
}
bool Type::can_be_callable() const {
	return is_any() or (_types.size() and some([&](std::shared_ptr<const Base_type> t) {
		return t->callable();
	}));
}
bool Type::is_void() const {
	return _types.size() == 0;
}
bool Type::is_template() const { return is_type<Template_type>(); }

bool Type::compatible(const Type& type) const {
	if (_types.size() == 0 or type._types.size() == 0) {
		return true;
	}
	if (is_any()) {
		return true;
	}
	if (this->is_primitive() && type.is_polymorphic()) {
		return false;
	}
	if (this->temporary and not type.temporary) {
		return false; // type not compatible with type&&
	}
	if (not this->constant and type.constant) {
		return false; // 'const type' not compatible with 'type'
	}
	if ((is_array() && type.is_array()) || (is_set() && type.is_set()) || (is_map() && type.is_map()) || (is_function() && type.is_function())) {
		return _types[0]->compatible(type._types[0].get());
	}
	if (_types[0] != type._types[0]) {
		// 'Integer' is compatible with 'Float'
		if (this->is_real() and type.is_integer()) {
			return true;
		}
		// 'Boolean' is compatible with 'Integer'
		if (this->is_integer() and type.is_bool()) {
			return true;
		}
		// 'Integer' is compatible with 'Long'
		if (this->is_long() and type.is_integer()) {
			return true;
		}
		// All numbers types are compatible with the base 'Number' type
		if (dynamic_cast<const Number_type*>(_types[0].get()) and (type.is_integer() or type.is_long() or type.is_real())) {
			return true;
		}
		return false;
	}
	return true;
}

void Type::implement(const Type& type) const {
	if (auto t = dynamic_cast<const Template_type*>(_types[0].get())) {
		t->implement(type);
	}
}
bool Type::is_implemented(const Type& type) const {
	return _types.size() and all([&](std::shared_ptr<const Base_type> t) {
		if (auto tpl = dynamic_cast<const Template_type*>(t.get())) {
			return not tpl->_implementation.is_void();
		}
		return false;
	});
}

bool Type::castable(Type type, bool strictCast) const {
	if (!_types.size()) return false;
	auto d = distance(type);
	return d >= 0 and (!strictCast or d < 100000);
}
bool Type::strictCastable(Type type) const {
	if (!_types.size()) return false;
	auto d = distance(type);
	return d >= 0 and d < 100;
}
int Type::distance(Type type) const {
	if (_types.size() == 0 and type._types.size() == 0) return 0;
	if (_types.size() == 0 or type._types.size() == 0) return -1;
	if (not temporary and type.temporary) return -1;
	auto t1 = fold()._types[0];
	auto t2 = type.fold()._types[0];
	return t1->distance(t2.get());
}

std::shared_ptr<const Base_type> Type::generate_new_placeholder_type() {
	u_int32_t character = 0x03B1 + placeholder_counter;
	char buff[5];
	u8_toutf8(buff, 5, &character, 1);
	auto type = std::make_shared<Placeholder_type>(std::string { buff });
	placeholder_counter++;
	Type::placeholder_types.push_back(type);
	return type;
}

Type Type::never() {
	return { raw_never(), false, false, false };
}
Type Type::null() {
	return { raw_null(), true, false, false };
}
Type Type::any() {
	return { raw_any(), false, false, false };
}
Type Type::const_any() {
	return { raw_any(), false, false, true };
}
Type Type::boolean() {
	return { raw_boolean(), true, false, false };
}
Type Type::const_boolean() {
	return { raw_boolean(), true, false, true };
}
Type Type::number() {
	return { raw_number(), false, false, false };
}
Type Type::const_number() {
	return { raw_number(), false, false, true };
}
Type Type::i8() {
	return { raw_i8(), false, false, false };
}
Type Type::integer() {
	return { raw_integer(), false, false, false };
}
Type Type::const_integer() {
	return { raw_integer(), false, false, true };
}
Type Type::long_() {
	return { raw_long(), false, false, false };
}
Type Type::const_long() {
	return { raw_long(), false, false, true };
}
Type Type::mpz() {
	return { raw_mpz(), false, false, false };
}
Type Type::tmp_mpz() {
	return { raw_mpz(), false, true, false };
}
Type Type::const_mpz() {
	return { raw_mpz(), false, false, true };
}
Type Type::string() {
	return { raw_string(), false, false, false };
}
Type Type::tmp_string() {
	return { raw_string(), false, true, false };
}
Type Type::const_string() {
	return { raw_string(), false, false, true };
}
Type Type::real() {
	return { raw_real(), false, false, false };
}
Type Type::const_real() {
	return { raw_real(), false, false, true };
}
Type Type::array(const Type element) {
	return { Array_type::create(element), false, false, false };
}
Type Type::const_array(const Type element) {
	return { Array_type::create(element), false, false, true };
}
Type Type::tmp_array(const Type element) {
	return { Array_type::create(element), false, true, false };
}
Type Type::object() {
	return { raw_object(), false, false, false };
}
Type Type::tmp_object() {
	return { raw_object(), false, true, false };
}
Type Type::set(const Type element) {
	return { std::make_shared<Set_type>(element), false, false, false };
}
Type Type::const_set(const Type element) {
	return { std::make_shared<Set_type>(element), false, false, true };
}
Type Type::tmp_set(const Type element) {
	return { std::make_shared<Set_type>(element), false, true, false };
}
Type Type::map(const Type key, const Type element) {
	return { std::make_shared<Map_type>(key, element), false, false, false };
}
Type Type::const_map(const Type key, const Type element) {
	return { std::make_shared<Map_type>(key, element), false, false, true };
}
Type Type::interval() {
	return { raw_interval(), false, false, false };
}
Type Type::const_interval() {
	return { raw_interval(), false, false, true };
}
Type Type::tmp_interval() {
	return { raw_interval(), false, true, false };
}
Type Type::fun(Type return_type, std::vector<Type> arguments, const Value* function) {
	return { std::make_shared<Function_type>(return_type, arguments, false, function), true, false, true };
}
Type Type::closure(Type return_type, std::vector<Type> arguments, const Value* function) {
	return { std::make_shared<Function_type>(return_type, arguments, true, function), true, false, true };
}
Type Type::structure(const std::string name, std::initializer_list<Type> types) {
	return { std::make_shared<Struct_type>(name, types), false, false, false };
}
Type Type::clazz(const std::string name) {
	if (_raw_class.find(name) == _raw_class.end()) {
		_raw_class.insert({name, std::make_shared<Class_type>(name) });
	}
	return { _raw_class.at(name), true, false, false };
}
Type Type::const_class(const std::string name) {
	if (_raw_class.find(name) == _raw_class.end()) {
		_raw_class.insert({name, std::make_shared<Class_type>(name) });
	}
	return { _raw_class.at(name), true, false, true };
}
Type Type::template_(std::string name) {
	return { std::make_shared<Template_type>(name), false, false, false };
}
bool Type::all(std::function<bool(std::shared_ptr<const Base_type>)> fun) const {
	return std::all_of(_types.begin(), _types.end(), fun);
}
bool Type::some(std::function<bool(std::shared_ptr<const Base_type>)> fun) const {
	return std::any_of(_types.begin(), _types.end(), fun);
}

const std::shared_ptr<Base_type> Type::raw_never() {
	if (!_raw_never) _raw_never = std::make_shared<Never_type>();
	return _raw_never;
}
const std::shared_ptr<Base_type> Type::raw_null() {
	if (!_raw_null) _raw_null = std::make_shared<Null_type>();
	return _raw_null;
}
const std::shared_ptr<Base_type> Type::raw_any() {
	if (!_raw_any) _raw_any = std::make_shared<Any_type>();
	return _raw_any;
}
const std::shared_ptr<Base_type> Type::raw_boolean() {
	if (!_raw_boolean) _raw_boolean = std::make_shared<Bool_type>();
	return _raw_boolean;
}
const std::shared_ptr<Base_type> Type::raw_number() {
	if (!_raw_number) _raw_number = std::make_shared<Number_type>();
	return _raw_number;
}
const std::shared_ptr<Base_type> Type::raw_i8() {
	if (!_raw_i8) _raw_i8 = std::make_shared<I8_type>();
	return _raw_i8;
}
const std::shared_ptr<Base_type> Type::raw_integer() {
	if (!_raw_integer) _raw_integer = std::make_shared<Integer_type>();
	return _raw_integer;
}
const std::shared_ptr<Base_type> Type::raw_long() {
	if (!_raw_long) _raw_long = std::make_shared<Long_type>();
	return _raw_long;
}
const std::shared_ptr<Base_type> Type::raw_real() {
	if (!_raw_real) _raw_real = std::make_shared<Real_type>();
	return _raw_real;
}
const std::shared_ptr<Base_type> Type::raw_mpz() {
	if (!_raw_mpz) _raw_mpz = std::make_shared<Mpz_type>();
	return _raw_mpz;
}
const std::shared_ptr<Base_type> Type::raw_string() {
	if (!_raw_string) _raw_string = std::make_shared<String_type>();
	return _raw_string;
}
const std::shared_ptr<Base_type> Type::raw_interval() {
	if (!_raw_interval) _raw_interval = std::make_shared<Interval_type>();
	return _raw_interval;
}
const std::shared_ptr<Base_type> Type::raw_object() {
	if (!_raw_object) _raw_object = std::make_shared<Object_type>();
	return _raw_object;
}

std::ostream& operator << (std::ostream& os, const Type& type) {
	if (type._types.size() == 0) {
		os << C_GREY << "void" << END_COLOR;
		return os;
	}
	if (type.constant) {
		os << BLUE_BOLD << "const:" << END_COLOR;
	}
	for (size_t i = 0; i < type._types.size(); ++i) {
		if (i > 0) { os << " | "; }
		type._types[i]->print(os);
	}
	if (type.temporary) {
		os << BLUE_BOLD << "&&" << END_COLOR;
	} else if (type.reference) {
		os << BLUE_BOLD << "&" << END_COLOR;
	}
	return os;
}

std::ostream& operator << (std::ostream& os, const std::vector<Type>& types) {
	os << "[";
	for (unsigned i = 0; i < types.size(); ++i) {
		if (i > 0) os << ", ";
		os << types[i];
	}
	os << "]";
	return os;
}

std::ostream& operator << (std::ostream& os, const llvm::Type* type) {
	std::string str;
	llvm::raw_string_ostream rso(str);
	type->print(rso);
	os << rso.str();
	return os;
}

}
