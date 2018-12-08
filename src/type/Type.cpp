#include <sstream>
#include <numeric>
#include "Type.hpp"
#include "../constants.h"
#include "../colors.h"
#include "../../lib/utf8.h"
#include "../vm/VM.hpp"
#include "RawType.hpp"
#include "../compiler/value/Function.hpp"

using namespace std;

namespace ls {

std::vector<const Base_type*> RawType::placeholder_types;

void RawType::clear_placeholder_types() {
	for (const auto& t : placeholder_types)
		delete t;
	placeholder_types.clear();
}

unsigned int Type::placeholder_counter = 0;

const Type Type::ANY(RawType::ANY);
const Type Type::CONST_ANY(RawType::ANY, false, false, true);
const Type Type::NULLL(RawType::NULLL, true);
const Type Type::BOOLEAN(RawType::BOOLEAN);
const Type Type::CONST_BOOLEAN(RawType::BOOLEAN, false, false, true);
const Type Type::NUMBER(RawType::NUMBER);
const Type Type::CONST_NUMBER(RawType::NUMBER, false, false, true);
const Type Type::INTEGER(RawType::INTEGER);
const Type Type::CONST_INTEGER(RawType::INTEGER, false, false, true);
const Type Type::MPZ(RawType::MPZ);
const Type Type::MPZ_TMP(RawType::MPZ, false, true);
const Type Type::LONG(RawType::LONG);
const Type Type::CONST_LONG(RawType::LONG, false, false, true);
const Type Type::REAL(RawType::REAL);
const Type Type::CONST_REAL(RawType::REAL, false, false, true);
const Type Type::STRING(RawType::STRING);
const Type Type::CONST_STRING(RawType::STRING, false, false, true);
const Type Type::STRING_TMP(RawType::STRING, false, true);
const Type Type::OBJECT(RawType::OBJECT);
const Type Type::OBJECT_TMP(RawType::OBJECT, false, true);

const Type Type::ARRAY = Type::array();
const Type Type::PTR_ARRAY = Type::array(Type::ANY);
const Type Type::CONST_ARRAY = Type::const_array();
const Type Type::INT_ARRAY = Type::array(Type::INTEGER);
const Type Type::REAL_ARRAY = Type::array(Type::REAL);
const Type Type::PTR_ARRAY_TMP = Type::tmp_array(Type::ANY);
const Type Type::INT_ARRAY_TMP = Type::tmp_array(Type::INTEGER);
const Type Type::REAL_ARRAY_TMP = Type::tmp_array(Type::REAL);
const Type Type::STRING_ARRAY = Type::array(Type::STRING);
const Type Type::CONST_PTR_ARRAY = Type::const_array(Type::ANY);
const Type Type::CONST_INT_ARRAY = Type::const_array(Type::INTEGER);
const Type Type::CONST_REAL_ARRAY = Type::const_array(Type::REAL);
const Type Type::CONST_STRING_ARRAY = Type::const_array(Type::STRING);

const Type Type::MAP = Type::map();
const Type Type::PTR_PTR_MAP = Type::map(Type::ANY, Type::ANY);
const Type Type::PTR_INT_MAP = Type::map(Type::ANY, Type::INTEGER);
const Type Type::PTR_REAL_MAP = Type::map(Type::ANY, Type::REAL);
const Type Type::REAL_PTR_MAP = Type::map(Type::REAL, Type::ANY);
const Type Type::REAL_INT_MAP = Type::map(Type::REAL, Type::INTEGER);
const Type Type::REAL_REAL_MAP = Type::map(Type::REAL, Type::REAL);
const Type Type::INT_PTR_MAP = Type::map(Type::INTEGER, Type::ANY);
const Type Type::INT_INT_MAP = Type::map(Type::INTEGER, Type::INTEGER);
const Type Type::INT_REAL_MAP = Type::map(Type::INTEGER, Type::REAL);
const Type Type::CONST_MAP = Type::const_map();
const Type Type::CONST_PTR_PTR_MAP = Type::const_map(Type::ANY, Type::ANY);
const Type Type::CONST_PTR_INT_MAP = Type::const_map(Type::ANY, Type::INTEGER);
const Type Type::CONST_PTR_REAL_MAP = Type::const_map(Type::ANY, Type::REAL);
const Type Type::CONST_REAL_PTR_MAP = Type::const_map(Type::REAL, Type::ANY);
const Type Type::CONST_REAL_INT_MAP = Type::const_map(Type::REAL, Type::INTEGER);
const Type Type::CONST_REAL_REAL_MAP = Type::const_map(Type::REAL, Type::REAL);
const Type Type::CONST_INT_PTR_MAP = Type::const_map(Type::INTEGER, Type::ANY);
const Type Type::CONST_INT_INT_MAP = Type::const_map(Type::INTEGER, Type::INTEGER);
const Type Type::CONST_INT_REAL_MAP = Type::const_map(Type::INTEGER, Type::REAL);

const Type Type::SET = Type::set();
const Type Type::PTR_SET = Type::set(Type::ANY);
const Type Type::INT_SET = Type::set(Type::INTEGER);
const Type Type::REAL_SET = Type::set(Type::REAL);
const Type Type::CONST_SET = Type::const_set();
const Type Type::CONST_INT_SET = Type::const_set(Type::INTEGER);
const Type Type::CONST_REAL_SET = Type::const_set(Type::REAL);

const Type Type::INTERVAL = Type::interval();
const Type Type::INTERVAL_TMP = Type::tmp_interval();
const Type Type::PTR_ARRAY_ARRAY = Type::array(Type::PTR_ARRAY);
const Type Type::REAL_ARRAY_ARRAY = Type::array(Type::REAL_ARRAY);
const Type Type::INT_ARRAY_ARRAY = Type::array(Type::INT_ARRAY);

const Type Type::FUNCTION(new Function_type({}, {}));
const Type Type::CLOSURE(new Function_type({}, {}, true));
const Type Type::CLASS(RawType::CLASS, true);
const Type Type::CONST_CLASS(RawType::CLASS, true, false, true);

const Type Type::STRING_ITERATOR = Type::iterator(Type::STRING);
const Type Type::INTERVAL_ITERATOR = Type::iterator(Type::INTERVAL);
const Type Type::SET_ITERATOR = Type::iterator(Type::PTR_SET);
const Type Type::INTEGER_ITERATOR = Type::structure("int_iterator", {
	Type::INTEGER,
	Type::INTEGER,
	Type::INTEGER
});
const Type Type::LONG_ITERATOR = Type::structure("long_iterator", {
	Type::LONG,
	Type::LONG,
	Type::INTEGER
});
const Type Type::MPZ_ITERATOR = Type::iterator(Type::MPZ);
const Type Type::INT_ARRAY_ITERATOR = Type::iterator(Type::INT_ARRAY);
const Type Type::REAL_ARRAY_ITERATOR = Type::iterator(Type::REAL_ARRAY);
const Type Type::PTR_ARRAY_ITERATOR = Type::iterator(Type::PTR_ARRAY);

Type::Type() {
	native = false;
}
Type::Type(std::initializer_list<const Base_type*> types) {
	_types = types;
}
Type::Type(std::initializer_list<Type> types) {
	for (const auto& t : types) {
		operator += (t);
	}
}
Type::Type(const Base_type* raw_type, bool native, bool temporary, bool constant) {
	_types.push_back(raw_type);
	this->native = native;
	this->temporary = temporary;
	this->constant = constant;
}

int Type::id() const {
	if (_types.size() == 0) { return 0; }
	return _types[0]->id();
}

bool Type::must_manage_memory() const {
	if (_types.size() == 0) { return false; }
	return !isNumber() and not native;
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
	if (_types.size() == 0) { return {}; }
	return _types[0]->element();
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
}
void Type::operator += (const Base_type* type) {
	for (const auto& t : _types) {
		if (type->operator == (t))
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
	if (is_null() or t2.is_null()) {
		return Type::NULLL;
	}
	if (!isNumber() and t2.isNumber()) {
		return Type::ANY;
	}
	if (!t2.isNumber() and isNumber()) {
		return Type::ANY;
	}
	if (is_any()) {
		return t2;
	}
	if (t2.is_any()) {
		return *this;
	}
	if (t2.compatible(*this)) {
		return t2;
	}
	if (compatible(t2)) {
		return *this;
	}
	return Type::ANY;
}

Type Type::fold() const {
	if (_types.size() == 1) return *this;
	return std::accumulate(_types.begin(), _types.end(), Type(), std::multiplies<>());
}

void Type::toJson(ostream& os) const {
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

std::string Type::clazz() const {
	if (_types.size() == 0) { return "Any"; }
	return fold()._types[0]->clazz();
}

bool Type::isNumber() const {
	return _types.size() && all([&](const Base_type* type) {
		return dynamic_cast<const Number_type*>(type) != nullptr || dynamic_cast<const Bool_type*>(type) != nullptr;
	});
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

bool Type::operator == (const Type& type) const {
	return _types.size() == type._types.size() && std::equal(_types.begin(), _types.end(), type._types.begin(), [&](const Base_type* t1, const Base_type* t2) {
		return t1->operator == (t2);
	});
}

bool Type::operator < (const Type& type) const {
	auto t1 = _types.size() ? _types[0] : nullptr;
	auto t2 = type._types.size() ? type._types[0] : nullptr;
	if ((void*) t1 != (void*) t2) {
		return (void*) t1 < (void*) t2;
	}
	if (temporary != type.temporary) {
		return temporary < type.temporary;
	}
	if (reference != type.reference) {
		return reference < type.reference;
	}
	if (native != type.native) {
		return native < type.native;
	}
	return false;
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

llvm::Type* Type::llvm_type() const {
	if (_types.size() == 0) {
		return llvm::Type::getVoidTy(LLVMCompiler::context);
	}
	return fold()._types[0]->llvm();
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
	return { new Pointer_type(*this) };
}

template <class T> bool Type::is_type() const {
	return _types.size() && all([&](const Base_type* type) {
		return dynamic_cast<const T*>(type) != nullptr;
	});
}
bool Type::is_any() const { return is_type<Any_type>(); }
bool Type::is_bool() const { return is_type<Bool_type>(); }
bool Type::is_number() const { return is_type<Number_type>(); }
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
bool Type::is_null() const { return is_type<Null_type>(); }
bool Type::is_class() const { return is_type<Class_type>(); }
bool Type::is_placeholder() const { return is_type<Placeholder_type>(); }
bool Type::is_pointer() const { return is_type<Pointer_type>(); }
bool Type::is_closure() const {
	return _types.size() && all([&](const Base_type* type) {
		auto f = dynamic_cast<const Function_type*>(type);
		return f && f->closure();
	});
}

/*
 * Can we convert type into this ?
 * {float}.compatible({int}) == true
 * {int*}.compatible({int}) == true
 * {int}.compatible({float}) == false
 * {int}.compatible({int*}) == false
 */
bool Type::compatible(const Type& type) const {
	if (_types.size() == 0 or type._types.size() == 0) {
		return true;
	}
	if (is_any()) {
		return true;
	}
	if (this->isNumber() && !type.isNumber()) {
		return false;
	}
	if (this->temporary and not type.temporary) {
		return false; // type not compatible with type&&
	}
	if (not this->constant and type.constant) {
		return false; // 'const type' not compatible with 'type'
	}
	if ((is_array() && type.is_array()) || (is_set() && type.is_set()) || (is_map() && type.is_map()) || (is_function() && type.is_function())) {
		return _types[0]->compatible(type._types[0]);
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
		if (_types[0] == RawType::NUMBER and (type.is_integer() or type.is_long() or type.is_real())) {
			return true;
		}
		return false;
	}
	return true;
}

bool Type::list_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual) {

	if (expected.size() != actual.size()) return false;

	for (size_t i = 0; i < expected.size(); ++i) {
		// Can we convert type actual[i] into type expected[i] ?
		if (not expected[i].compatible(actual[i])) return false;
	}
	return true;
}

bool Type::may_be_compatible(const Type& type) const {
	if (compatible(type)) {
		return true;
	}
	// Example: Number.abs(number*) => we allow to call with a unknown pointer
	if (!isNumber() and type == Type::ANY) {
		return true;
	}
	return false;
}

bool Type::list_may_be_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual) {
	if (expected.size() != actual.size()) return false;
	for (size_t i = 0; i < expected.size(); ++i) {
		if (not expected[i].may_be_compatible(actual[i])) return false;
	}
	return true;
}

Type Type::generate_new_placeholder_type() {
	Type type;
	u_int32_t character = 0x03B1 + placeholder_counter;
	char buff[5];
	u8_toutf8(buff, 5, &character, 1);
	type._types.push_back(new Placeholder_type(std::string { buff }));
	placeholder_counter++;
	RawType::placeholder_types.push_back(type._types[0]);
	return type;
}

Type Type::array(const Type element) {
	return { new Array_type(element) };
}
Type Type::const_array(const Type element) {
	return { new Array_type(element), false, false, true };
}
Type Type::tmp_array(const Type element) {
	return { new Array_type(element), false, true };
}
Type Type::set(const Type element) {
	return { new Set_type(element) };
}
Type Type::const_set(const Type element) {
	return { new Set_type(element), false, false, true };
}
Type Type::tmp_set(const Type element) {
	return { new Set_type(element), false, true };
}
Type Type::map(const Type key, const Type element) {
	return { new Map_type(key, element) };
}
Type Type::const_map(const Type key, const Type element) {
	return { new Map_type(key, element), false, false, true };
}
Type Type::interval() {
	return { new Interval_type() };
}
Type Type::tmp_interval() {
	return { new Interval_type(), false, true };
}
Type Type::fun(Type return_type, std::vector<Type> arguments, const Function* function) {
	return { new Function_type(return_type, arguments, false, function), false, false, true };
}
Type Type::closure(Type return_type, std::vector<Type> arguments, const Function* function) {
	return { new Function_type(return_type, arguments, true, function), false, false, true };
}
Type Type::iterator(const Type container) {
	return { new Iterator_type(container) };
}
Type Type::structure(const std::string name, std::initializer_list<Type> types) {
	return { new Struct_type(name, types) };
}
bool Type::all(std::function<bool(const Base_type*)> fun) const {
	return std::all_of(_types.begin(), _types.end(), fun);
}

ostream& operator << (ostream& os, const Type& type) {
	if (type._types.size() == 0) {
		os << C_GREY << "void" << END_COLOR;
		return os;
	}
	if (type.constant) {
		os << BLUE_BOLD << "const:" << END_COLOR;
	}
	for (int i = 0; i < type._types.size(); ++i) {
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

ostream& operator << (ostream& os, const std::vector<Type>& types) {
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
