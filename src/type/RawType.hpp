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
#include "Integer_type.hpp"
#include "Placeholder_type.hpp"
#include "Struct_type.hpp"
#include "Pointer_type.hpp"

namespace ls {

class RawType {
public:
	static const std::shared_ptr<const Any_type> ANY;
	static const std::shared_ptr<const Null_type> NULLL;
	static const std::shared_ptr<const Bool_type> BOOLEAN;
	static const std::shared_ptr<const Number_type> NUMBER;
	static const std::shared_ptr<const Mpz_type> MPZ;
	static const std::shared_ptr<const Integer_type> INTEGER;
	static const std::shared_ptr<const Long_type> LONG;
	static const std::shared_ptr<const Real_type> REAL;
	static const std::shared_ptr<const String_type> STRING;
	static const std::shared_ptr<const Object_type> OBJECT;
	static const std::shared_ptr<const Class_type> CLASS;

	
};

}

#endif