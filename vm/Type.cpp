#include "Type.hpp"

using namespace std;

const BaseRawType* const RawType::UNKNOWN = new BaseRawType();
const NullRawType* const RawType::NULLL = new NullRawType();
const BooleanRawType* const RawType::BOOLEAN = new BooleanRawType();
const NumberRawType* const RawType::NUMBER = new NumberRawType();
const IntegerRawType* const RawType::INTEGER = new IntegerRawType();
const LongRawType* const RawType::LONG = new LongRawType();
const FloatRawType* const RawType::FLOAT = new FloatRawType();
const StringRawType* const RawType::STRING = new StringRawType();
const ArrayRawType* const RawType::ARRAY = new ArrayRawType();
const ObjectRawType* const RawType::OBJECT = new ObjectRawType();
const FunctionRawType* const RawType::FUNCTION = new FunctionRawType();
const ClassRawType* const RawType::CLASS = new ClassRawType();

const Type Type::UNKNOWN(RawType::UNKNOWN, Nature::UNKNOWN);

const Type Type::NEUTRAL(RawType::UNKNOWN, Nature::VALUE);
const Type Type::VALUE(RawType::UNKNOWN, Nature::VALUE);
const Type Type::POINTER(RawType::UNKNOWN, Nature::POINTER);

const Type Type::NULLL(RawType::NULLL, Nature::VALUE);
const Type Type::BOOLEAN(RawType::BOOLEAN, Nature::VALUE);
const Type Type::BOOLEAN_P(RawType::BOOLEAN, Nature::POINTER);
const Type Type::NUMBER(RawType::NUMBER, Nature::POINTER);
const Type Type::INTEGER(RawType::INTEGER, Nature::VALUE);
const Type Type::INTEGER_P(RawType::INTEGER, Nature::POINTER);
const Type Type::LONG(RawType::LONG, Nature::VALUE);
const Type Type::FLOAT(RawType::FLOAT, Nature::VALUE);
const Type Type::FLOAT_P(RawType::FLOAT, Nature::POINTER);
const Type Type::STRING(RawType::STRING, Nature::POINTER);
const Type Type::OBJECT(RawType::OBJECT, Nature::POINTER);
const Type Type::ARRAY(RawType::ARRAY, Nature::POINTER);
const Type Type::INT_ARRAY(RawType::ARRAY, Nature::POINTER, Type::INTEGER);

const Type Type::FUNCTION(RawType::FUNCTION, Nature::VALUE);
const Type Type::FUNCTION_P(RawType::FUNCTION, Nature::POINTER);
const Type Type::CLASS(RawType::CLASS, Nature::POINTER);



Type::Type() {
	raw_type = RawType::UNKNOWN;
	nature = Nature::UNKNOWN;
	clazz = "?";
	homogeneous = true;
}

Type::Type(const Type& type) {
	this->raw_type = type.raw_type;
	this->nature = type.nature;
	this->return_types = type.return_types;
	this->arguments_types = type.arguments_types;
	this->clazz = raw_type->getName();
	this->element_type = type.element_type;
	this->homogeneous = type.homogeneous;
}

Type::Type(const BaseRawType* raw_type, Nature nature) {
	this->raw_type = raw_type;
	this->nature = nature;
	this->clazz = raw_type->getName();
	this->homogeneous = true;
}

Type::Type(const BaseRawType* raw_type, Nature nature, const Type& elements_type) {
	this->raw_type = raw_type;
	this->nature = nature;
	this->clazz = raw_type->getName();
	this->homogeneous = true;
	this->setElementType(elements_type);
}

Type Type::getReturnType() const {
	if (return_types.size() == 0) {
		return Type::UNKNOWN;
	}
	return return_types[0];
}

void Type::setReturnType(Type type) {
	if (return_types.size() == 0) {
		return_types.push_back(Type::UNKNOWN);
	}
	return_types[0] = type;
}

void Type::addArgumentType(Type type) {
	arguments_types.push_back(type);
}

void Type::setArgumentType(unsigned int index, Type type) {
	while (arguments_types.size() <= index) {
		arguments_types.push_back(Type::UNKNOWN);
	}
	arguments_types[index] = type;
}

/*
 * By default, all arguments are type INTEGER, but if we see it's not always
 * a integer, it will switch to UNKNOWN
 */
const Type Type::getArgumentType(const unsigned int index) const {
	if (index >= arguments_types.size()) {
		return Type::UNKNOWN;
	}
	return arguments_types[index];
}

const vector<Type> Type::getArgumentTypes() const {
	return arguments_types;
}

const Type Type::getElementType() const {
	if (element_type.size() == 0) {
		return Type::UNKNOWN;
	}
	return element_type[0];
}

void Type::setElementType(Type type) {
	if (element_type.size() == 0) {
		element_type.push_back(type);
	} else {
		element_type[0] = type;
	}
}

bool Type::isHomogeneous() const {
	return homogeneous;
}

/*
 *
 */
bool Type::will_take(const int i, const Type& arg_type) {

//	cout << "before will_take: " << *this << endl;
//	cout << "will_take " << arg_type << endl;

	Type current_type = getArgumentType(i);

	/*
	if (current_type.nature == Nature::MIXED) {
		return; // No change, it is still mixed
	}
	*/

	if (current_type.nature == Nature::UNKNOWN) {
		setArgumentType(i, arg_type);
	} else {
		if (current_type.nature == Nature::VALUE and arg_type.nature == Nature::POINTER) {
			setArgumentType(i, Type(RawType::UNKNOWN, Nature::POINTER));
			return true;
		}
	}

//	cout << "after will_take: " << *this << endl;
	return false;
}

bool Type::will_take_element(const Type& element_type) {

	if (raw_type != RawType::ARRAY) {
		return false;
	}

	Type current = getElementType();

	if (current == element_type) {
		return false;
	}

	setElementType(element_type);
	return true;
}

Type Type::mix(const Type& x) const {

	Type type(*this);
	if (x.raw_type == RawType::FLOAT) type.raw_type = RawType::FLOAT;
	if (x.nature == Nature::POINTER) type.nature = Nature::POINTER;
	return type;
}

bool Type::operator == (const Type& type) const {

	if (this->raw_type != type.raw_type) return false;
	if (this->nature != type.nature) return false;

	if (this->return_types != type.return_types) return false;
	if (this->arguments_types != type.arguments_types) return false;

	if (this->homogeneous != type.homogeneous) return false;
	if (this->element_type.size() > 0 and type.element_type.size() > 0 and
		this->element_type != type.element_type) return false;

	return true;
}

bool Type::compatible(const Type& type) const {

//	cout << "COMPATIBLE : " << type << " with " << *this << endl;

	if (this->nature == Nature::VALUE && type.nature == Nature::POINTER) return false;

	if (this->raw_type != type.raw_type) {

		if (this->raw_type == RawType::UNKNOWN) return true;
		if (this->raw_type == RawType::NUMBER and (
			type.raw_type == RawType::INTEGER or
			type.raw_type == RawType::LONG or
			type.raw_type == RawType::FLOAT
		)) return true;

		return false;
	}

	if (this->raw_type == RawType::ARRAY) {
		return this->getElementType().compatible(type.getElementType());
	}

	return true;
}


bool Type::operator != (const Type& type) const {
	return !this->operator == (type);
}

bool Type::list_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual) {

	if (expected.size() != actual.size()) return false;

	for (unsigned a = 0; a < expected.size(); ++a) {

		if (not expected.at(a).compatible(actual.at(a))) return false;
//		cout << "YES" << endl;
	}
	return true;
}

bool Type::more_specific(const Type& neww, const Type& old) {

//	cout << "MORE SPECIFIC : " << neww << " than " << old << endl;

	if (neww.raw_type != old.raw_type and neww.raw_type->derived_from(old.raw_type)) {
//		cout << "DERIVED" << endl;
		return true;
	}

	if (neww.raw_type == RawType::ARRAY and old.raw_type == RawType::ARRAY) {

//		cout << "new element type : " << neww.getElementType() << endl;
//		cout << "old element type : " << old.getElementType() << endl;

		if (Type::more_specific(neww.getElementType(), old.getElementType())) {
//			cout << "YES" << endl;
			return true;
		}
	}

	if (neww.raw_type == RawType::FUNCTION and old.raw_type == RawType::FUNCTION) {

//		cout << "more_specific function" << endl;
//		cout << "new element type : " << neww.getArgumentType(0) << endl;
//		cout << "old element type : " << old.getArgumentType(0) << endl;

		// TODO only the first arg
		if (Type::more_specific(neww.getArgumentType(0), old.getArgumentType(0))) {
//			cout << "YES" << endl;
			return true;
		}
	}
	return false;
}

bool Type::list_more_specific(const std::vector<Type>& old, const std::vector<Type>& neww) {

//	cout << "MORE SPECIFIC" << endl;
//	cout << "[TEST] " << Type::more_specific(Type::INTEGER, Type::UNKNOWN) << endl;

	if (old.size() != neww.size()) return false;

	for (unsigned a = 0; a < old.size(); ++a) {
//		cout << "CHECK : " << neww.at(a) << " ||| " << old.at(a) << endl;
		if (Type::more_specific(neww.at(a), old.at(a))) {
//			cout << "MORE SPECIFIC : " << neww.at(a) << " ||| " << old.at(a) << endl;
			return true;
		}
	}
	return false;
}

void Type::toJson(ostream& os) const {
	os << "{\"type\":\"" << raw_type->getJsonName() << "\"";

	if (raw_type == RawType::FUNCTION) {
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

ostream& operator << (ostream& os, const Type& type) {

	os << "{n: " << Type::get_nature_name(type.nature)
	<< ", t: "<< type.raw_type->getName();

	if (type.raw_type == RawType::FUNCTION) {
		os << ", args: [";
		for (unsigned t = 0; t < type.arguments_types.size(); ++t) {
			if (t > 0) os << ", ";
			os << type.arguments_types[t];
		}
		os << "]";
		os << ", returns: " << type.getReturnType();
	}
	if (type.raw_type == RawType::ARRAY) {
		if (type.homogeneous) {
			os << ", elements: " << type.getElementType();
		} else {
			os << ", heterogeneous";
		}
	}
	os << "}";
	return os;
}

string Type::get_nature_name(const Nature& nature) {
	switch (nature) {
	case Nature::MIXED:
		return "MIXED";
	case Nature::POINTER:
		return "POINTER";
	case Nature::REFERENCE:
		return "REFERENCE";
	case Nature::UNKNOWN:
		return "UNKNOWN";
	case Nature::VALUE:
		return "VALUE";
	default:
		return "??";
	}
}
