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

const std::vector<const Type*> Type::empty_types;
std::map<std::set<std::shared_ptr<const Base_type>>, const Type*> Type::compound_types;
std::map<const Type*, const Type*> Type::array_types;
std::map<const Type*, const Type*> Type::const_array_types;
std::map<const Type*, const Type*> Type::tmp_array_types;
std::map<const Type*, const Type*> Type::set_types;
std::map<const Type*, const Type*> Type::const_set_types;
std::map<const Type*, const Type*> Type::tmp_set_types;
std::map<std::pair<const Type*, const Type*>, const Type*> Type::map_types;
std::map<std::pair<const Type*, const Type*>, const Type*> Type::const_map_types;
std::map<std::pair<const Type*, const Type*>, const Type*> Type::tmp_map_types;
std::map<const Type*, const Type*> Type::pointer_types;
std::map<const Type*, const Type*> Type::temporary_types;
std::map<const Type*, const Type*> Type::not_temporary_types;
std::map<const Type*, const Type*> Type::const_types;
std::map<const Type*, const Type*> Type::not_const_types;

const Type* const Type::void_ = new Type {};
const Type* const Type::boolean = new Type { raw_boolean(), true };
const Type* const Type::const_boolean = Type::boolean->add_constant();
const Type* const Type::number = new Type { raw_number(), false };
const Type* const Type::const_number = Type::number->add_constant();
const Type* const Type::i8 = new Type { raw_i8(), false };
const Type* const Type::integer = new Type { raw_integer(), false };
const Type* const Type::const_integer = Type::integer->add_constant();
const Type* const Type::long_ = new Type { raw_long(), false };
const Type* const Type::const_long = Type::long_->add_constant();
const Type* const Type::mpz = new Type { raw_mpz(), false };
const Type* const Type::tmp_mpz = Type::mpz->add_temporary();
const Type* const Type::const_mpz = Type::mpz->add_constant();
const Type* const Type::mpz_ptr = Type::mpz->pointer();
const Type* const Type::tmp_mpz_ptr = Type::mpz_ptr->add_temporary();

const Type* const Type::never = new Type { raw_never(), false };
const Type* const Type::any = new Type { raw_any(), false };
const Type* const Type::const_any = Type::any->add_constant();
const Type* const Type::null = new Type { raw_null(), true };
const Type* const Type::string = new Type { raw_string(), false };
const Type* const Type::tmp_string = Type::string->add_temporary();
const Type* const Type::const_string = Type::string->add_constant();
const Type* const Type::real = new Type { raw_real(), false };
const Type* const Type::const_real = Type::real->add_constant();
const Type* const Type::interval = new Type { raw_interval(), false };
const Type* const Type::const_interval = Type::interval->add_constant();
const Type* const Type::tmp_interval = Type::interval->add_temporary();
const Type* const Type::object = new Type { raw_object(), false };
const Type* const Type::tmp_object = Type::object->add_temporary();

Type::Type() {
	folded = this;
}
Type::Type(std::set<std::shared_ptr<const Base_type>> types, const Type* folded) : folded(folded) {
	for (const auto& t : types) _types.push_back(t);
}
Type::Type(std::shared_ptr<const Base_type> raw_type, bool native) {
	_types.push_back(raw_type);
	this->native = native;
	folded = this;
}

int Type::id() const {
	if (is_void()) { return 0; }
	return fold()->_types[0]->id();
}

bool Type::must_manage_memory() const {
	if (is_void()) { return false; }
	return is_polymorphic() and not native;
}

const Type* Type::return_type() const {
	if (is_void()) { return Type::void_; }
	return _types[0]->return_type();
}
const Type* Type::argument(size_t index) const {
	if (is_void()) { return void_; }
	return _types[0]->argument(index);
}
const std::vector<const Type*>& Type::arguments() const {
	if (is_void()) { return empty_types; }
	return _types[0]->arguments();
}
const Type* Type::element() const {
	if (is_void()) { return Type::void_; }
	if (_types.size() == 1) return _types[0]->element();
	return fold()->element();
}
const Type* Type::key() const {
	if (is_void()) { return Type::void_; }
	return _types[0]->key();
}
const Type* Type::member(int i) const {
	if (is_void()) { return Type::void_; }
	return _types[0]->member(i);
}

const Type* Type::operator + (std::shared_ptr<const Base_type> type) const {
	auto t = new Type();
	t->operator += (this);
	t->operator += (type);
	return t;
}
const Type* Type::operator + (const Type* type) const {
	if (is_void()) return type;
	if (type->is_void()) return this;
	return Type::compound({this, type});
}
void Type::operator += (const Type* type) {
	for (const auto& t : type->_types) {
		operator += (t);
	}
}
void Type::operator += (std::shared_ptr<const Base_type> type) {
	_types.push_back(type);
}
const Type* Type::operator * (const Type* t2) const {
	if (this == void_) return t2;
	if (t2 == void_ or this == t2) return this;
	if (is_polymorphic() and t2->is_primitive()) {
		return any;
	}
	if (t2->is_polymorphic() and is_primitive()) {
		return any;
	}
	// Temporary, to be removed when compatible() is removed
	if ((is_bool() and t2->is_integer()) or (is_integer() and t2->is_bool())) {
		return any;
	}
	if (t2->compatible(this)) {
		return t2;
	}
	if (compatible(t2)) {
		return this;
	}
	if (is_array() and t2->is_array()) {
		if (element()->is_polymorphic() and t2->element()->is_polymorphic()) {
			return array(any);
		}
	}
	return any;
}

const Type* Type::fold() const {
	assert(folded != nullptr);
	if (_types.size() <= 1) return this;
	return folded;
}

void Type::toJson(std::ostream& os) const {
	os << "{\"type\":\"" << getJsonName() << "\"";

	if (is_function()) {
		os << ",\"args\":[";
		for (unsigned t = 0; t < arguments().size(); ++t) {
			if (t > 0) os << ",";
			argument(t)->toJson(os);
		}
		os << "]";
		os << ",\"return\":";
		return_type()->toJson(os);
	}
	os << "}";
}
std::string Type::getJsonName() const {
	if (is_void()) {
		return "?";
	}
	return _types[0]->getJsonName();
}

std::string Type::to_string() const {
	std::ostringstream oss;
	oss << this;
	return oss.str();
}

std::string Type::class_name() const {
	if (is_void()) { return ""; }
	return fold()->_types[0]->clazz();
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

const Type* Type::add_temporary() const {
	if (temporary) return this;
	if (constant) return not_constant()->add_temporary();
	auto i = temporary_types.find(this);
	if (i != temporary_types.end()) return i->second;
	auto type = new Type { *this };
	type->temporary = true;
	temporary_types.insert({ this, type });
	not_temporary_types.insert({ type, this });
	return type;
}
const Type* Type::not_temporary() const {
	if (not temporary) return this;
	auto i = not_temporary_types.find(this);
	if (i != not_temporary_types.end()) return i->second;
	auto type = new Type { *this };
	type->temporary = false;
	not_temporary_types.insert({ this, type });
	temporary_types.insert({ type, this });
	return type;
}
const Type* Type::add_constant() const {
	if (constant) return this;
	if (temporary) return not_temporary()->add_constant();
	auto i = const_types.find(this);
	if (i != const_types.end()) return i->second;
	auto type = new Type { *this };
	type->constant = true;
	const_types.insert({ this, type });
	not_const_types.insert({ type, this });
	return type;
}
const Type* Type::not_constant() const {
	if (not constant) return this;
	auto i = not_const_types.find(this);
	if (i != not_const_types.end()) return i->second;
	auto type = new Type { *this };
	type->constant = false;
	not_const_types.insert({ this, type });
	const_types.insert({ type, this });
	return type;
}

llvm::Type* Type::llvm_type(const Compiler& c) const {
	if (is_void()) {
		return llvm::Type::getVoidTy(c.getContext());
	}
	return fold()->_types[0]->llvm(c);
}

const Type* Type::iterator() const {
	if (_types.size() > 0) {
		return fold()->_types[0]->iterator();
	}
	assert(false && "No iterator for void");
}
const Type* Type::pointer() const {
	auto i = pointer_types.find(this);
	if (i != pointer_types.end()) return i->second;
	if (temporary) {
		auto type = this->not_temporary()->pointer()->add_temporary();
		pointer_types.insert({ this, type });
		return type;
	} else if (constant) {
		auto type = this->not_constant()->pointer()->add_constant();
		pointer_types.insert({ this, type });
		return type;
	} else {
		auto type = new Type { std::make_shared<Pointer_type>(this) };
		pointer_types.insert({ this, type });
		return type;
	}
}
const Type* Type::pointed() const {
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
			return t->_implementation->is_type<T>();
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
bool Type::is_number() const { return castable(Type::number, true); }
bool Type::can_be_number() const {
	return some([&](std::shared_ptr<const Base_type> type) {
		return type->distance(Type::number->_types[0].get()) >= 0;
	});
}
bool Type::can_be_numeric() const {
	return is_any() or can_be_bool() or can_be_number();
}
bool Type::is_real() const { return folded == Type::real; }
bool Type::is_integer() const { return folded == Type::integer; }
bool Type::is_long() const { return folded == Type::long_; }
bool Type::is_mpz() const { return is_type<Mpz_type>(); }
bool Type::is_mpz_ptr() const { return folded == Type::mpz_ptr; }
bool Type::is_string() const { return folded == Type::string; }
bool Type::is_array() const { return is_type<Array_type>(); }
bool Type::is_set() const { return is_type<Set_type>(); }
bool Type::is_interval() const { return folded == Type::interval; }
bool Type::is_map() const { return is_type<Map_type>(); }
bool Type::is_function() const { return is_type<Function_type>(); }
bool Type::is_object() const { return folded == Type::object; }
bool Type::is_never() const { return folded == Type::never; }
bool Type::is_null() const { return folded == Type::null; }
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
	// TODO extends all polymorphic types from Polymorphic_type (Any_type) to improve check
	return _types.size() and fold()->all([&](std::shared_ptr<const Base_type> t) {
		return dynamic_cast<const String_type*>(t.get()) != nullptr
			or dynamic_cast<const Array_type*>(t.get()) != nullptr
			or dynamic_cast<const Set_type*>(t.get()) != nullptr
			or dynamic_cast<const Map_type*>(t.get()) != nullptr
			or dynamic_cast<const Interval_type*>(t.get()) != nullptr
			or dynamic_cast<const Any_type*>(t.get()) != nullptr
			or dynamic_cast<const Function_type*>(t.get()) != nullptr
			or dynamic_cast<const Class_type*>(t.get()) != nullptr
			or dynamic_cast<const Object_type*>(t.get()) != nullptr
			or dynamic_cast<const Null_type*>(t.get()) != nullptr;
	});
}
bool Type::is_primitive() const {
	return _types.size() && all([&](std::shared_ptr<const Base_type> t) {
		return dynamic_cast<const Integer_type*>(t.get()) != nullptr
			or dynamic_cast<const Mpz_type*>(t.get()) != nullptr 
			or dynamic_cast<const Long_type*>(t.get()) != nullptr 
			or dynamic_cast<const Real_type*>(t.get()) != nullptr 
			or dynamic_cast<const Bool_type*>(t.get()) != nullptr
			or (dynamic_cast<const Pointer_type*>(t.get()) != nullptr and ((Pointer_type*) t.get())->pointed()->is_mpz());
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
	return this == Type::void_;
}
bool Type::is_template() const { return is_type<Template_type>(); }

bool Type::compatible(const Type* type) const {
	if (is_void() or type->is_void()) {
		return true;
	}
	if (is_any()) {
		return true;
	}
	if (this->is_primitive() && type->is_polymorphic()) {
		return false;
	}
	if (this->temporary and not type->temporary) {
		return false; // type not compatible with type&&
	}
	if (not this->constant and type->constant) {
		return false; // 'const type' not compatible with 'type'
	}
	if ((is_array() && type->is_array()) || (is_set() && type->is_set()) || (is_map() && type->is_map()) || (is_function() && type->is_function())) {
		return _types[0]->compatible(type->_types[0].get());
	}
	if (_types[0] != type->_types[0]) {
		// 'Integer' is compatible with 'Float'
		if (this->is_real() and type->is_integer()) {
			return true;
		}
		// 'Boolean' is compatible with 'Integer'
		if (this->is_integer() and type->is_bool()) {
			return true;
		}
		// 'Integer' is compatible with 'Long'
		if (this->is_long() and type->is_integer()) {
			return true;
		}
		// All numbers types are compatible with the base 'Number' type
		if (dynamic_cast<const Number_type*>(_types[0].get()) and (type->is_integer() or type->is_long() or type->is_real())) {
			return true;
		}
		return false;
	}
	return true;
}

void Type::implement(const Type* type) const {
	if (auto t = dynamic_cast<const Template_type*>(_types[0].get())) {
		t->implement(type);
	}
}
bool Type::is_implemented(const Type* type) const {
	return _types.size() and all([&](std::shared_ptr<const Base_type> t) {
		if (auto tpl = dynamic_cast<const Template_type*>(t.get())) {
			return not tpl->_implementation->is_void();
		}
		return false;
	});
}

bool Type::castable(const Type* type, bool strictCast) const {
	if (!_types.size()) return false;
	auto d = distance(type);
	return d >= 0 and (!strictCast or d < 100000);
}
bool Type::strictCastable(const Type* type) const {
	if (!_types.size()) return false;
	auto d = distance(type);
	return d >= 0 and d < 100;
}
int Type::distance(const Type* type) const {
	if (is_void() and type->is_void()) return 0;
	if (is_void() or type->is_void()) return -1;
	if (not temporary and type->temporary) return -1;
	auto f1 { fold() };
	auto f2 { type->fold() };
	const auto& t1 = f1->_types[0];
	const auto& t2 = f2->_types[0];
	return t1->distance(t2.get());
}
const Type* Type::without_placeholders() const {
	return this;
}

const Type* Type::generate_new_placeholder_type() {
	u_int32_t character = 0x03B1 + placeholder_counter;
	char buff[5];
	u8_toutf8(buff, 5, &character, 1);
	auto type = std::make_shared<Placeholder_type>(std::string { buff });
	placeholder_counter++;
	Type::placeholder_types.push_back(type);
	return new Type { type };
}

const Type* Type::array(const Type* element) {
	auto i = array_types.find(element);
	if (i != array_types.end()) return i->second;
	auto type = new Type { Array_type::create(element), false };
	array_types.insert({element, type});
	return type;
}
const Type* Type::const_array(const Type* element) {
	auto i = const_array_types.find(element);
	if (i != const_array_types.end()) return i->second;
	auto type = array(element)->add_constant();
	const_array_types.insert({element, type});
	return type;
}
const Type* Type::tmp_array(const Type* element) {
	auto i = tmp_array_types.find(element);
	if (i != tmp_array_types.end()) return i->second;
	auto type = array(element)->add_temporary();
	tmp_array_types.insert({element, type});
	return type;
}
const Type* Type::set(const Type* element) {
	auto i = set_types.find(element);
	if (i != set_types.end()) return i->second;
	auto type = new Type { Set_type::create(element), false };
	set_types.insert({element, type});
	return type;
}
const Type* Type::const_set(const Type* element) {
	auto i = const_set_types.find(element);
	if (i != const_set_types.end()) return i->second;
	auto type = set(element)->add_constant();
	const_set_types.insert({element, type});
	return type;
}
const Type* Type::tmp_set(const Type* element) {
	auto i = tmp_set_types.find(element);
	if (i != tmp_set_types.end()) return i->second;
	auto type = set(element)->add_temporary();
	tmp_set_types.insert({element, type});
	return type;
}
const Type* Type::map(const Type* key, const Type* element) {
	auto i = map_types.find({key, element});
	if (i != map_types.end()) return i->second;
	auto type = new Type { std::make_shared<Map_type>(key, element), false };
	map_types.insert({{key, element}, type});
	return type;
}
const Type* Type::const_map(const Type* key, const Type* element) {
	auto i = const_map_types.find({key, element});
	if (i != const_map_types.end()) return i->second;
	auto type = map(key, element)->add_constant();
	const_map_types.insert({{key, element}, type});
	return type;
}
const Type* Type::tmp_map(const Type* key, const Type* element) {
	auto i = tmp_map_types.find({key, element});
	if (i != tmp_map_types.end()) return i->second;
	auto type = map(key, element)->add_temporary();
	tmp_map_types.insert({{key, element}, type});
	return type;
}
const Type* Type::fun(const Type* return_type, std::vector<const Type*> arguments, const Value* function) {
	auto t = new Type { std::make_shared<Function_type>(return_type, arguments, false, function), true };
	t->constant = true;
	return t;
}
const Type* Type::closure(const Type* return_type, std::vector<const Type*> arguments, const Value* function) {
	auto t = new Type { std::make_shared<Function_type>(return_type, arguments, true, function), true };
	t->constant = true;
	return t;
}
const Type* Type::structure(const std::string name, std::initializer_list<const Type*> types) {
	return new Type { std::make_shared<Struct_type>(name, types), false };
}
const Type* Type::clazz(const std::string name) {
	if (_raw_class.find(name) == _raw_class.end()) {
		_raw_class.insert({name, std::make_shared<Class_type>(name) });
	}
	return new Type { _raw_class.at(name), true };
}
const Type* Type::const_class(const std::string name) {
	return clazz(name)->add_constant();
}
const Type* Type::template_(std::string name) {
	return new Type { std::make_shared<Template_type>(name), false };
}
const Type* Type::compound(std::initializer_list<const Type*> types) {
	if (types.size() == 1) return *types.begin();
	std::set<std::shared_ptr<const Base_type>> base;
	auto folded = Type::void_;
	for (const auto& t : types) {
		for (const auto& bt : t->_types) base.insert(bt);
		folded = folded->operator * (t);
	}
	if (base.size() == 1) return *types.begin();
	auto i = compound_types.find(base);
	if (i != compound_types.end()) return i->second;
	auto type = new Type { base, folded };
	compound_types.insert({ base, type });
	return type;
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

std::ostream& operator << (std::ostream& os, const Type* type) {
	if (type->is_void()) {
		os << C_GREY << "void" << END_COLOR;
		return os;
	}
	if (type->constant) {
		os << BLUE_BOLD << "const:" << END_COLOR;
	}
	for (size_t i = 0; i < type->_types.size(); ++i) {
		if (i > 0) { os << " | "; }
		type->_types[i]->print(os);
	}
	if (type->temporary) {
		os << BLUE_BOLD << "&&" << END_COLOR;
	} else if (type->reference) {
		os << BLUE_BOLD << "&" << END_COLOR;
	}
	return os;
}

std::ostream& operator << (std::ostream& os, const std::vector<const Type*>& types) {
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
