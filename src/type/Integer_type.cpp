#include "Integer_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"
#include "Long_type.hpp"
#include "Any_type.hpp"
#include "Real_type.hpp"
#include "Mpz_type.hpp"
#include "Bool_type.hpp"

namespace ls {

Type Integer_type::element() const {
	return Type::INTEGER;
}
Type Integer_type::iterator() const {
	return Type::INTEGER_ITERATOR;
}
bool Integer_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Integer_type*>(type) != nullptr;
}
int Integer_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 5; }
	if (dynamic_cast<const Number_type*>(type)) { return 4; }
	if (dynamic_cast<const Mpz_type*>(type)) { return 3; }
	if (dynamic_cast<const Real_type*>(type)) { return 2; }
	if (dynamic_cast<const Long_type*>(type)) { return 1; }
	if (dynamic_cast<const Integer_type*>(type)) { return 0; }
	if (dynamic_cast<const Bool_type*>(type)) { return 100; }
	return -1;
}
llvm::Type* Integer_type::llvm() const {
	return llvm::Type::getInt32Ty(LLVMCompiler::context);
}
std::string Integer_type::clazz() const {
	return "Number";
}
std::ostream& Integer_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "int" << END_COLOR;
	return os;
}

}