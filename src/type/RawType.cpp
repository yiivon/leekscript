#include "RawType.hpp"

namespace ls {

const std::shared_ptr<const Any_type> RawType::ANY = std::make_shared<Any_type>();
const std::shared_ptr<const Null_type> RawType::NULLL = std::make_shared<Null_type>();
const std::shared_ptr<const Bool_type> RawType::BOOLEAN = std::make_shared<Bool_type>();
const std::shared_ptr<const Number_type> RawType::NUMBER = std::make_shared<Number_type>();
const std::shared_ptr<const Integer_type> RawType::INTEGER = std::make_shared<Integer_type>();
const std::shared_ptr<const Mpz_type> RawType::MPZ = std::make_shared<Mpz_type>();
const std::shared_ptr<const Long_type> RawType::LONG = std::make_shared<Long_type>();
const std::shared_ptr<const Real_type> RawType::REAL = std::make_shared<Real_type>();
const std::shared_ptr<const String_type> RawType::STRING = std::make_shared<String_type>();
const std::shared_ptr<const Object_type> RawType::OBJECT = std::make_shared<Object_type>();
const std::shared_ptr<const Class_type> RawType::CLASS = std::make_shared<Class_type>();

}