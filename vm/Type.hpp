#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <iostream>
using namespace std;

enum class RawType {
	UNKNOWN, NULLL, BOOLEAN, INTEGER, LONG, FLOAT, STRING, OBJECT, ARRAY, FUNCTION, CLASS
};

enum class Nature {
	UNKNOWN, MIXED, VALUE, POINTER, REFERENCE
};


class Type {
public:

	RawType raw_type;
	Nature nature;
	string clazz;
	vector<Type> element_type;
	bool homogeneous;
	vector<Type> return_types;
	vector<Type> arguments_types;

	Type();
	Type(const Type& type);
	Type(RawType raw_type, Nature nature);

	Type getReturnType() const;
	void setReturnType(Type type);

	void addArgumentType(Type type);
	void setArgumentType(const unsigned int index, Type type);
	const Type getArgumentType(const unsigned int index) const;
	const vector<Type> getArgumentTypes() const;
	const Type getElementType() const;
	void setElementType(Type);
	const bool isHomogeneous() const;

	string get_class_name() const;

	bool will_take(const int i, const Type& arg_type);
	Type mix(const Type& x) const;

	bool operator == (const Type&) const;
	bool operator != (const Type&) const;

	static const Type NEUTRAL;
	static const Type VALUE;
	static const Type POINTER;

	static const Type UNKNOWN;
	static const Type NULLL;
	static const Type BOOLEAN;
	static const Type BOOLEAN_P;
	static const Type INTEGER;
	static const Type INTEGER_P;
	static const Type LONG;
	static const Type FLOAT;
	static const Type FLOAT_P;
	static const Type STRING;
	static const Type OBJECT;
	static const Type ARRAY;
	static const Type FUNCTION;
	static const Type FUNCTION_P;
	static const Type CLASS;

	static string get_raw_type_name(const RawType& raw_type);
	static string get_nature_name(const Nature& nature);
};

ostream& operator << (ostream&, const Type&);

#endif
