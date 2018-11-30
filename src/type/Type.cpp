#include "Type.hpp"
#include "../constants.h"
#include "../colors.h"
#include <sstream>
#include "../../lib/utf8.h"
#include "../vm/VM.hpp"
#include "RawType.hpp"

using namespace std;

namespace ls {

llvm::StructType* Type::LLVM_LSVALUE_TYPE;
llvm::Type* Type::LLVM_LSVALUE_TYPE_PTR;
llvm::Type* Type::LLVM_LSVALUE_TYPE_PTR_PTR;
llvm::Type* Type::LLVM_MPZ_TYPE;
llvm::Type* Type::LLVM_MPZ_TYPE_PTR;
llvm::StructType* Type::LLVM_VECTOR_TYPE;
llvm::Type* Type::LLVM_VECTOR_TYPE_PTR;
llvm::StructType* Type::LLVM_VECTOR_INT_TYPE;
llvm::Type* Type::LLVM_VECTOR_INT_TYPE_PTR;
llvm::StructType* Type::LLVM_VECTOR_REAL_TYPE;
llvm::Type* Type::LLVM_VECTOR_REAL_TYPE_PTR;
llvm::StructType* Type::LLVM_FUNCTION_TYPE;
llvm::Type* Type::LLVM_FUNCTION_TYPE_PTR;
llvm::StructType* Type::LLVM_INTEGER_ITERATOR_TYPE;

std::vector<const BaseRawType*> RawType::placeholder_types;

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

const Type Type::FUNCTION(RawType::FUNCTION, false, false, true);
const Type Type::CONST_FUNCTION(RawType::FUNCTION, false, true, true);
const Type Type::CLOSURE(RawType::CLOSURE, false, false, true);
const Type Type::CLASS(RawType::CLASS, true);
const Type Type::CONST_CLASS(RawType::CLASS, true, false, true);

const Type Type::STRING_ITERATOR = Type::iterator(Type::STRING);
const Type Type::INTERVAL_ITERATOR = Type::iterator(Type::INTERVAL);
const Type Type::SET_ITERATOR = Type::iterator(Type::PTR_SET);
const Type Type::INTEGER_ITERATOR = Type::iterator(Type::INTEGER);
const Type Type::LONG_ITERATOR = Type::iterator(Type::LONG);
const Type Type::MPZ_ITERATOR = Type::iterator(Type::MPZ);
const Type Type::INT_ARRAY_ITERATOR = Type::iterator(Type::INT_ARRAY);
const Type Type::REAL_ARRAY_ITERATOR = Type::iterator(Type::REAL_ARRAY);
const Type Type::PTR_ARRAY_ITERATOR = Type::iterator(Type::PTR_ARRAY);

Type::Type() {
	native = false;
}

Type::Type(const BaseRawType* raw_type, bool native, bool temporary, bool constant) {
	_types.push_back(raw_type);
	this->native = native;
	this->temporary = temporary;
	this->constant = constant;
}

int Type::id() const {
	if (_types.size() == 0) { return 0; }
	return _types[0]->id();
}
const BaseRawType* Type::raw() const {
	if (_types.size() == 0) { return nullptr; }
	return _types[0];
}

bool Type::must_manage_memory() const {
	if (_types.size() == 0) { return false; }
	return !isNumber() and not native;
}

Type Type::getReturnType() const {
	if (return_types.size() == 0) {
		return Type::ANY;
	}
	return return_types[0];
}

void Type::setReturnType(Type type) {
	if (return_types.size() == 0) {
		return_types.push_back({});
	}
	return_types[0] = type;
}

void Type::addArgumentType(Type type) {
	arguments_types.push_back(type);
}

void Type::setArgumentType(size_t index, Type type, bool has_default) {
	while (arguments_types.size() <= index) {
		arguments_types.push_back(Type::ANY);
	}
	arguments_types[index] = type;

	while (arguments_has_default.size() <= index) {
		arguments_has_default.push_back(false);
	}
	arguments_has_default[index] = has_default;
}

/*
 * By default, all arguments are type INTEGER, but if we see it's not always
 * a integer, it will switch to UNKNOWN
 */
const Type Type::getArgumentType(size_t index) const {
	if (index >= arguments_types.size()) {
		return Type::ANY;
	}
	return arguments_types[index];
}

const std::vector<Type> Type::getArgumentTypes() const {
	return arguments_types;
}

bool Type::argumentHasDefault(size_t index) const {
	if (index >= arguments_has_default.size()) {
		return false;
	}
	return arguments_has_default[index];
}

const Type Type::getElementType() const {
	if (_types.size() == 0) { return {}; }
	return _types[0]->element();
}
const Type Type::getKeyType() const {
	if (_types.size() == 0) { return {}; }
	return _types[0]->key();
}

bool Type::will_take(const std::vector<Type>& args_type) {
	bool changed = false;
	for (size_t i = 0; i < args_type.size(); ++i) {
		Type current_type = getArgumentType(i);
		if (current_type.isNumber() and !args_type[i].isNumber()) {
			setArgumentType(i, Type::ANY);
			changed = true;
		}
	}
	return changed;
}

void Type::add(const Type type) {
	for (const auto& t : type._types) {
		add(t);
	}
}

void Type::add(const BaseRawType* type) {
	for (const auto& t : _types) {
		if (type == t)
			return;
	}
	_types.push_back(type);
}

void Type::toJson(ostream& os) const {
	os << "{\"type\":\"" << getJsonName() << "\"";

	if (raw() == RawType::FUNCTION) {
		os << ",\"args\":[";
		for (unsigned t = 0; t < arguments_types.size(); ++t) {
			if (t > 0) os << ",";
			arguments_types[t].toJson(os);
		}
		os << "]";
		os << ",\"return\":";
		getReturnType().toJson(os);
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

std::string Type::getClass() const {
	if (_types.size() == 0) {
		return "?";
	}
	return _types[0]->getClass();
}

bool Type::isNumber() const {
	return dynamic_cast<const NumberRawType*>(raw()) != nullptr || raw() == RawType::BOOLEAN;
}

bool Type::iterable() const {
	return raw()->iterable();
}

bool Type::is_container() const {
	return raw()->is_container();
}

bool Type::operator == (const Type& type) const {
	if (is_array() or is_set() or is_map()) {
		return raw()->operator == (type.raw());
	}
	return raw() == type.raw() &&
			// native == type.native &&
			// temporary == type.temporary &&
			// reference == type.reference &&
			((raw() != RawType::FUNCTION and raw() != RawType::CLOSURE) ||
				(return_types == type.return_types &&
				arguments_types == type.arguments_types));
}

bool Type::operator < (const Type& type) const {
	if ((void*) raw() != (void*) type.raw()) {
		return (void*) raw() < (void*) type.raw();
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
	return _types[0]->llvm();
}

Type Type::add_pointer() const {
	Type new_type = *this;
	new_type.pointer = true;
	return new_type;
}

Type Type::iteratorType() const {
	if (is_array()) {
		if (getElementType() == Type::INTEGER) return Type::INT_ARRAY_ITERATOR;
		if (getElementType() == Type::REAL) return Type::REAL_ARRAY_ITERATOR;
		else if (getElementType() == Type::ANY) return Type::PTR_ARRAY_ITERATOR;
	}
	assert(false && "No iterator type available");
}

bool Type::is_array() const {
	if (_types.size() == 0) { return false; }
	return dynamic_cast<const Array_type*>(_types[0]) != nullptr;
}
bool Type::is_set() const {
	if (_types.size() == 0) { return false; }
	return dynamic_cast<const Set_type*>(_types[0]) != nullptr;
}
bool Type::is_interval() const {
	if (_types.size() == 0) { return false; }
	return dynamic_cast<const Interval_type*>(_types[0]) != nullptr;
}
bool Type::is_map() const {
	if (_types.size() == 0) { return false; }
	return dynamic_cast<const Map_type*>(_types[0]) != nullptr;
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
	if (this->isNumber() && !type.isNumber()) {
		return false;
	}
	if (this->temporary and not type.temporary) {
		return false; // type not compatible with type&&
	}
	if (not this->constant and type.constant) {
		return false; // 'const type' not compatible with 'type'
	}
	if ((is_array() && is_array()) || (is_set() && is_set()) || (is_map() && type.is_map())) {
		return raw()->compatible(type.raw());
	}
	if (this->raw() != type.raw()) {
		// Every type is compatible with 'Unknown' type
		if (this->raw() == RawType::ANY) {
			return true;
		}
		// 'Integer' is compatible with 'Float'
		if (this->raw() == RawType::REAL and type.raw() == RawType::INTEGER) {
			return true;
		}
		// 'Boolean' is compatible with 'Integer'
		if (this->raw() == RawType::INTEGER and type.raw() == RawType::BOOLEAN) {
			return true;
		}
		// 'Integer' is compatible with 'Long'
		if (this->raw() == RawType::LONG and type.raw() == RawType::INTEGER) {
			return true;
		}
		// All numbers types are compatible with the base 'Number' type
		if (this->raw() == RawType::NUMBER and (
			type.raw() == RawType::INTEGER or
			type.raw() == RawType::LONG or
			type.raw() == RawType::REAL
		)) return true;

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

bool Type::list_more_specific(const std::vector<Type>& old, const std::vector<Type>& neww) {

	if (old.size() != neww.size()) return false;

	for (size_t i = 0; i < old.size(); ++i) {
		if (Type::more_specific(old[i], neww[i])) {
			return true;
		}
	}
	return false;
}

bool Type::more_specific(const Type& old, const Type& neww) {
	if (neww.raw() != old.raw()) {
		if (old.raw() == RawType::ANY) {
			return true;
		}
		if (old.raw() == RawType::NUMBER && (neww.raw() == RawType::INTEGER || neww.raw() == RawType::LONG || neww.raw() == RawType::REAL)) {
			return true;
		}
		if (old.raw() == RawType::REAL && (neww.raw() == RawType::INTEGER || neww.raw() == RawType::LONG)) {
			return true;
		}
		if (old.raw() == RawType::LONG && neww.raw() == RawType::INTEGER) {
			return true;
		}
	}
	if ((neww.is_array() and old.is_array()) || (neww.is_set() and old.is_set())) {
		if (Type::more_specific(old.getElementType(), neww.getElementType())) {
			return true;
		}
	}
	if (neww.is_map() and old.is_map()) {
		if (Type::more_specific(old.getKeyType(), neww.getKeyType()) || Type::more_specific(old.getElementType(), neww.getElementType())) {
			return true;
		}
	}
	if ((neww.raw() == RawType::FUNCTION or neww.raw() == RawType::CLOSURE) and (old.raw() == RawType::FUNCTION or old.raw() == RawType::CLOSURE)) {
		if (Type::more_specific(old.getArgumentType(0), neww.getArgumentType(0))) { //! TODO only the first arg
			return true;
		}
	}
	if (not old.temporary and neww.temporary) {
		return true;
	}
	return false;
}

Type Type::get_compatible_type(const Type& t1, const Type& t2) {
	if (t1._types.size() == 0) {
		return t2;
	}
	if (t2._types.size() == 0) {
		return t1;
	}
	if (t1 == t2) {
		return t1;
	}
	if (t1.raw() == RawType::NULLL or t2.raw() == RawType::NULLL) {
		return Type::NULLL;
	}
	if (!t1.isNumber() and t2.isNumber()) {
		return Type::ANY;
	}
	if (!t2.isNumber() and t1.isNumber()) {
		return Type::ANY;
	}
	if (t1.raw() == RawType::ANY) {
		return t2;
	}
	if (t2.raw() == RawType::ANY) {
		return t1;
	}
	if (t2.compatible(t1)) {
		return t2;
	}
	if (t1.compatible(t2)) {
		return t1;
	}
	if (t1.is_array() and t2.is_array()) {
		return array(get_compatible_type(t1.getElementType(), t2.getElementType()));
	}
	return Type::ANY;
}

Type Type::generate_new_placeholder_type() {
	Type type;
	u_int32_t character = 0x03B1 + placeholder_counter;
	char buff[5];
	u8_toutf8(buff, 5, &character, 1);
	type._types.push_back(new PlaceholderRawType(std::string { buff }));
	placeholder_counter++;
	RawType::placeholder_types.push_back(type.raw());
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
Type Type::fun() {
	return { RawType::FUNCTION };
}
Type Type::iterator(const Type container) {
	return { new Iterator_type(container) };
}

ostream& operator << (ostream& os, const Type& type) {
	if (type._types.size() == 0) {
		os << C_GREY << "void" << END_COLOR;
		return os;
	}
	if (type.constant) {
		os << BLUE_BOLD << "const" << END_COLOR;
	}
	for (int i = 0; i < type._types.size(); ++i) {
		if (i > 0) { os << " | "; }
		type._types[i]->print(os);
	}
	return os;

	auto color = type.isNumber() ? C_GREEN : C_RED;
	os << color;

	if (type.raw() == RawType::ANY) {
		os << BLUE_BOLD;
		if (type.constant) os << "const:";
		if (type == Type::INT_ARRAY_ITERATOR) {
			os << "iterator<array<int>>";
		} else if (type == Type::REAL_ARRAY_ITERATOR) {
			os << "iterator<array<real>>";
		} else if (type == Type::PTR_ARRAY_ITERATOR) {
			os << "iterator<array<*>>";
		} else {
			os << "any";
		}
	} else if (type.raw() == RawType::FUNCTION || type.raw() == RawType::CLOSURE) {
		os << BLUE_BOLD;
		if (type.constant) os << "const:";
		os << (type.raw() == RawType::FUNCTION ? "fun(" : "closure(");
		for (unsigned t = 0; t < type.arguments_types.size(); ++t) {
			if (t > 0) os << ", ";
			os << type.arguments_types[t];
			os << BLUE_BOLD;
		}
		os << BLUE_BOLD;
		os << ") → " << type.getReturnType();
	} else if (type.raw() == RawType::STRING || type.raw() == RawType::CLASS
		|| type.raw() == RawType::OBJECT || type.raw() == RawType::NULLL
		|| type.is_interval()) {
		os << BLUE_BOLD;
		if (type.constant) os << "const:";
		os << type.raw()->getName();
	} else if (type.is_array() || type.is_set()) {
		os << BLUE_BOLD;
		if (type.constant) os << "const:";
		os << type.raw()->getName();
		os << "<" << type.getElementType() << BLUE_BOLD << ">";
	} else if (type.is_map()) {
		os << BLUE_BOLD;
		if (type.constant) os << "const:";
		os << type.raw()->getName();
		os << "<" << type.getKeyType() << BLUE_BOLD
			<< ", " << type.getElementType() << BLUE_BOLD << ">";
	} else {
		if (type.constant) os << "const:";
		os << type.raw()->getName();
	}
	if (type.temporary && type.raw() != RawType::REAL) {
		os << "&&";
	} else if (type.reference) {
		os << "&";
	}
	os << END_COLOR;

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
