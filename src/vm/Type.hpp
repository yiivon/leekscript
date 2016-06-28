#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <iostream>

namespace ls {

/*
enum class RawType {
	UNKNOWN, NULLL, BOOLEAN, NUMBER, INTEGER, LONG, FLOAT, STRING, OBJECT, ARRAY, FUNCTION, CLASS
};
*/

enum class Nature {
	UNKNOWN, MIXED, VALUE, POINTER, REFERENCE
};

class BaseRawType {
public:
	virtual ~BaseRawType() {};
	virtual const std::string getName() const { return "?"; };
	virtual const std::string getJsonName() const { return "?"; };
	virtual bool operator == (const BaseRawType*) { return true; }
	virtual bool derived_from(const BaseRawType*) const { return false; }
};

class VoidRawType : public BaseRawType {
public:
	~VoidRawType() {};
	virtual const std::string getName() const { return "Void"; };
	virtual const std::string getJsonName() const { return "void"; };
	virtual bool operator == (const BaseRawType*) { return false; }
	bool operator == (const VoidRawType*) { return true; }
	virtual bool derived_from(const BaseRawType*) const { return true; }
};

class NullRawType : public BaseRawType {
public:
	~NullRawType() {};
	virtual const std::string getName() const { return "Null"; };
	virtual const std::string getJsonName() const { return "null"; };
	virtual bool operator == (const BaseRawType*) { return false; }
	bool operator == (const NullRawType*) { return true; }
	virtual bool derived_from(const BaseRawType*) const { return true; }
};

class BooleanRawType : public BaseRawType {
public:
	~BooleanRawType() {}
	virtual const std::string getName() const { return "Boolean"; };
	virtual const std::string getJsonName() const { return "boolean"; };
	bool operator == (const BaseRawType*) { return false; }
	bool operator == (const BooleanRawType*) { return true; }
	virtual bool derived_from(const BaseRawType*) const { return true; }
};

class NumberRawType : public BaseRawType {
public:
	~NumberRawType() {}
	const std::string getName() const { return "Number"; };
	const std::string getJsonName() const { return "number"; };
	bool operator == (const BaseRawType*) { return false; }
	bool operator == (const NumberRawType*) { return true; }
	virtual bool derived_from(const BaseRawType*) const { return true; }
};

class IntegerRawType : public NumberRawType {
public:
	~IntegerRawType() {}
	const std::string getName() const { return "Integer"; };
	const std::string getJsonName() const { return "number"; };
	bool operator == (const BaseRawType*) { return false; }
	bool operator == (const IntegerRawType*) { return true; }
	bool operator > (const NumberRawType*) { return true; }
	virtual bool derived_from(const NumberRawType*) const { return true; }
};

class LongRawType : public NumberRawType {
public:
	~LongRawType() {}
	const std::string getName() const { return "Long"; };
	const std::string getJsonName() const { return "number"; };
	bool operator == (const BaseRawType*) { return false; }
	bool operator == (const LongRawType*) { return true; }
	virtual bool derived_from(const NumberRawType*) const { return true; }
};

class FloatRawType : public NumberRawType {
public:
	~FloatRawType() {}
	const std::string getName() const { return "Float"; };
	const std::string getJsonName() const { return "number"; };
	bool operator == (const BaseRawType*) { return false; }
	bool operator == (const FloatRawType*) { return true; }
	virtual bool derived_from(const NumberRawType*) const { return true; }
};

class StringRawType : public BaseRawType {
public:
	~StringRawType() {}
	virtual const std::string getName() const { return "String"; };
	virtual const std::string getJsonName() const { return "string"; };
	bool operator == (const BaseRawType*) { return false; }
	bool operator == (const StringRawType*) { return true; }
	virtual bool derived_from(const BaseRawType*) const { return true; }
};

class ArrayRawType : public BaseRawType {
public:
	~ArrayRawType() {}
	const std::string getName() const { return "Array"; };
	const std::string getJsonName() const { return "array"; };
	virtual bool operator == (const BaseRawType*) { return false; }
	bool operator == (const ArrayRawType*) { return true; }
	virtual bool derived_from(const BaseRawType*) const { return true; }
};

class ObjectRawType : public BaseRawType {
public:
	~ObjectRawType() {}
	const std::string getName() const { return "Object"; };
	const std::string getJsonName() const { return "object"; };
	virtual bool operator == (const BaseRawType*) { return false; }
	bool operator == (const ObjectRawType*) { return true; }
	virtual bool derived_from(const BaseRawType*) const { return true; }
};

class FunctionRawType : public BaseRawType {
public:
	~FunctionRawType() {}
	const std::string getName() const { return "Function"; };
	const std::string getJsonName() const { return "function"; };
	bool operator == (const BaseRawType*) { return false; }
	bool operator == (const FunctionRawType*) { return true; }
	virtual bool derived_from(const BaseRawType*) const { return true; }
};

class ClassRawType : public BaseRawType {
public:
	~ClassRawType() {}
	const std::string getName() const { return "Class"; };
	const std::string getJsonName() const { return "class"; };
	bool operator == (const BaseRawType*) { return false; }
	bool operator == (const ClassRawType*) { return true; }
	virtual bool derived_from(const BaseRawType*) const { return true; }
};

class RawType {
public:
	static const BaseRawType* const UNKNOWN;
	static const VoidRawType* const VOID;
	static const NullRawType* const NULLL;
	static const BooleanRawType* const BOOLEAN;
	static const NumberRawType* const NUMBER;
	static const IntegerRawType* const INTEGER;
	static const LongRawType* const LONG;
	static const FloatRawType* const FLOAT;
	static const StringRawType* const STRING;
	static const ArrayRawType* const ARRAY;
	static const ObjectRawType* const OBJECT;
	static const FunctionRawType* const FUNCTION;
	static const ClassRawType* const CLASS;
};

class Type {
public:

	const BaseRawType* raw_type;
	Nature nature;
	bool native; // A C++ objects, memory management is done outside the language
	std::string clazz;
	std::vector<Type> element_type;
	std::vector<Type> return_types;
	std::vector<Type> arguments_types;

	Type();
	Type(const Type& type);
	Type(const BaseRawType* raw_type, Nature nature);
	Type(const BaseRawType* raw_type, Nature nature, bool native);
	Type(const BaseRawType* raw_type, Nature nature, const Type& elements_type);
	Type(const BaseRawType* raw_type, Nature nature, const Type& elements_type, bool native);

	bool must_manage_memory();

	Type getReturnType() const;
	void setReturnType(Type type);

	void addArgumentType(Type type);
	void setArgumentType(const unsigned int index, Type type);
	const Type getArgumentType(const unsigned int index) const;
	const std::vector<Type> getArgumentTypes() const;
	const Type getElementType() const;
	void setElementType(Type);
	bool isHomogeneous() const;

	std::string get_class_name() const;

	bool will_take(const int i, const Type& arg_type);
	bool will_take_element(const Type& arg_type);
	Type mix(const Type& x) const;
	bool compatible(const Type& type) const;

	bool operator == (const Type&) const;
	bool operator != (const Type&) const;

	void toJson(std::ostream&) const;

	static const Type VOID;
	static const Type NEUTRAL;
	static const Type VALUE;
	static const Type POINTER;

	static const Type UNKNOWN;
	static const Type NULLL;
	static const Type BOOLEAN;
	static const Type BOOLEAN_P;
	static const Type NUMBER;
	static const Type INTEGER;
	static const Type INTEGER_P;
	static const Type LONG;
	static const Type FLOAT;
	static const Type FLOAT_P;
	static const Type STRING;
	static const Type OBJECT;
	static const Type ARRAY;
	static const Type INT_ARRAY;
	static const Type FLOAT_ARRAY;
	static const Type FUNCTION;
	static const Type FUNCTION_P;
	static const Type CLASS;

	static std::string get_nature_name(const Nature& nature);
	static bool list_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual);
	static bool list_more_specific(const std::vector<Type>& old, const std::vector<Type>& neww);
	static bool more_specific(const Type& old, const Type& neww);
	static Type get_compatible_type(Type& t1, Type& t2);
};

std::ostream& operator << (std::ostream&, const Type&);

}

#endif
