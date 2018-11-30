#include "RawType.hpp"

namespace ls {

const AnyRawType RawType::_ANY;
const NullRawType RawType::_NULL;
const BooleanRawType RawType::_BOOLEAN;
const NumberRawType RawType::_NUMBER;
const MpzRawType RawType::_MPZ;
const IntegerRawType RawType::_INTEGER;
const LongRawType RawType::_LONG;
const FloatRawType RawType::_REAL;
const StringRawType RawType::_STRING;
const ObjectRawType RawType::_OBJECT;
const FunctionRawType RawType::_FUNCTION;
const ClosureRawType RawType::_CLOSURE;
const ClassRawType RawType::_CLASS;

const AnyRawType* const RawType::ANY = &_ANY;
const NullRawType* const RawType::NULLL = &_NULL;
const BooleanRawType* const RawType::BOOLEAN = &_BOOLEAN;
const NumberRawType* const RawType::NUMBER = &_NUMBER;
const IntegerRawType* const RawType::INTEGER = &_INTEGER;
const MpzRawType* const RawType::MPZ = &_MPZ;
const LongRawType* const RawType::LONG = &_LONG;
const FloatRawType* const RawType::REAL = &_REAL;
const StringRawType* const RawType::STRING = &_STRING;
const ObjectRawType* const RawType::OBJECT = &_OBJECT;
const FunctionRawType* const RawType::FUNCTION = &_FUNCTION;
const ClosureRawType* const RawType::CLOSURE = &_CLOSURE;
const ClassRawType* const RawType::CLASS = &_CLASS;

}