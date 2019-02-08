#include "Number_type.hpp"
#include "../colors.h"
#include <iostream>
#include "Integer_type.hpp"
#include "Long_type.hpp"
#include "Real_type.hpp"
#include "Bool_type.hpp"
#include "Any_type.hpp"
#include "Mpz_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Number_type::Number_type() {}

bool Number_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Number_type*>(type);
}
int Number_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1; }
	if (dynamic_cast<const Number_type*>(type)) { return 0; }
	if (dynamic_cast<const Mpz_type*>(type)) { return 100; }
	if (dynamic_cast<const Real_type*>(type)) { return 200; }
	if (dynamic_cast<const Long_type*>(type)) { return 300; }
	if (dynamic_cast<const Integer_type*>(type)) { return 400; }
	if (dynamic_cast<const Bool_type*>(type)) { return 500; }
	return -1;
}
std::string Number_type::clazz() const {
	return "Number";
}
llvm::Type* Number_type::llvm() const {
	assert(false);
}
std::ostream& Number_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "number" << END_COLOR;
	return os;
}

}