#ifndef RAW_TYPE
#define RAW_TYPE

#include "Any_type.hpp"
#include "Array_type.hpp"
#include "Function_type.hpp"
#include "Class_type.hpp"
#include "Object_type.hpp"
#include "Class_type.hpp"
#include "Interval_type.hpp"
#include "String_type.hpp"
#include "Long_type.hpp"
#include "Int_type.hpp"
#include "Bool_type.hpp"
#include "Mpz_type.hpp"
#include "Real_type.hpp"
#include "Null_type.hpp"
#include "Map_type.hpp"
#include "Iterator_type.hpp"

namespace ls {

class RawType {
public:
	static const AnyRawType _ANY;
	static const NullRawType _NULL;
	static const BooleanRawType _BOOLEAN;
	static const NumberRawType _NUMBER;
	static const MpzRawType _MPZ;
	static const IntegerRawType _INTEGER;
	static const LongRawType _LONG;
	static const FloatRawType _REAL;
	static const StringRawType _STRING;
	static const Array_type _ARRAY;
	static const ObjectRawType _OBJECT;
	static const FunctionRawType _FUNCTION;
	static const ClosureRawType _CLOSURE;
	static const ClassRawType _CLASS;

	static const AnyRawType* const ANY;
	static const NullRawType* const NULLL;
	static const BooleanRawType* const BOOLEAN;
	static const NumberRawType* const NUMBER;
	static const MpzRawType* const MPZ;
	static const IntegerRawType* const INTEGER;
	static const LongRawType* const LONG;
	static const FloatRawType* const REAL;
	static const StringRawType* const STRING;
	static const ObjectRawType* const OBJECT;
	static const FunctionRawType* const FUNCTION;
	static const ClosureRawType* const CLOSURE;
	static const ClassRawType* const CLASS;

	static std::vector<const BaseRawType*> placeholder_types;
	static void clear_placeholder_types();
};

}

#endif