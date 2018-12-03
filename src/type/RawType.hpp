#ifndef RAW_TYPE
#define RAW_TYPE

#include "Any_type.hpp"
#include "Array_type.hpp"
#include "Set_type.hpp"
#include "Function_type.hpp"
#include "Class_type.hpp"
#include "Object_type.hpp"
#include "Class_type.hpp"
#include "Interval_type.hpp"
#include "String_type.hpp"
#include "Long_type.hpp"
#include "Bool_type.hpp"
#include "Mpz_type.hpp"
#include "Real_type.hpp"
#include "Null_type.hpp"
#include "Map_type.hpp"
#include "Iterator_type.hpp"
#include "Integer_type.hpp"

namespace ls {

class RawType {
public:
	static const Any_type _ANY;
	static const Null_type _NULL;
	static const Bool_type _BOOLEAN;
	static const Number_type _NUMBER;
	static const Mpz_type _MPZ;
	static const Integer_type _INTEGER;
	static const Long_type _LONG;
	static const Real_type _REAL;
	static const String_type _STRING;
	static const Array_type _ARRAY;
	static const Object_type _OBJECT;
	static const Class_type _CLASS;

	static const Any_type* const ANY;
	static const Null_type* const NULLL;
	static const Bool_type* const BOOLEAN;
	static const Number_type* const NUMBER;
	static const Mpz_type* const MPZ;
	static const Integer_type* const INTEGER;
	static const Long_type* const LONG;
	static const Real_type* const REAL;
	static const String_type* const STRING;
	static const Object_type* const OBJECT;
	static const Class_type* const CLASS;

	static std::vector<const Base_type*> placeholder_types;
	static void clear_placeholder_types();
};

}

#endif