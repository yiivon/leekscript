#include "Long_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"
#include "Number_type.hpp"
#include "Any_type.hpp"
#include "Real_type.hpp"
#include "Mpz_type.hpp"
#include "Bool_type.hpp"
#include "Integer_type.hpp"

namespace ls {

Type Long_type::element() const {
	return Type::INTEGER;
}
Type Long_type::iterator() const {
	return Type::LONG_ITERATOR;
}
bool Long_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Long_type*>(type) != nullptr;
}
int Long_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 6; }
	if (dynamic_cast<const Number_type*>(type)) { return 5; }
	if (dynamic_cast<const Mpz_type*>(type)) { return 4; }
	if (dynamic_cast<const Real_type*>(type)) { return 1; }
	if (dynamic_cast<const Long_type*>(type)) { return 0; }
	if (dynamic_cast<const Integer_type*>(type)) { return 100; }
	if (dynamic_cast<const Bool_type*>(type)) { return 101; }
	return -1;
}
llvm::Type* Long_type::llvm() const {
	return llvm::Type::getInt64Ty(LLVMCompiler::context);
}
std::string Long_type::clazz() const {
	return "Number";
}
std::ostream& Long_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "long" << END_COLOR;
	return os;
}

}