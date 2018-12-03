#include "RawType.hpp"

namespace ls {

const Any_type RawType::_ANY;
const Null_type RawType::_NULL;
const Bool_type RawType::_BOOLEAN;
const Number_type RawType::_NUMBER;
const Mpz_type RawType::_MPZ;
const Integer_type RawType::_INTEGER;
const Long_type RawType::_LONG;
const Real_type RawType::_REAL;
const String_type RawType::_STRING;
const Object_type RawType::_OBJECT;
const Class_type RawType::_CLASS;

const Any_type* const RawType::ANY = &_ANY;
const Null_type* const RawType::NULLL = &_NULL;
const Bool_type* const RawType::BOOLEAN = &_BOOLEAN;
const Number_type* const RawType::NUMBER = &_NUMBER;
const Integer_type* const RawType::INTEGER = &_INTEGER;
const Mpz_type* const RawType::MPZ = &_MPZ;
const Long_type* const RawType::LONG = &_LONG;
const Real_type* const RawType::REAL = &_REAL;
const String_type* const RawType::STRING = &_STRING;
const Object_type* const RawType::OBJECT = &_OBJECT;
const Class_type* const RawType::CLASS = &_CLASS;

}