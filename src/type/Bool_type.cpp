#include "Bool_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"
#include "Integer_type.hpp"
#include "Long_type.hpp"
#include "Real_type.hpp"
#include "Bool_type.hpp"
#include "Number_type.hpp"
#include "Any_type.hpp"
#include "Mpz_type.hpp"

namespace ls {

bool Bool_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Bool_type*>(type) != nullptr;
}
int Bool_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 6; }
	if (dynamic_cast<const Number_type*>(type)) { return 5; }
	if (dynamic_cast<const Mpz_type*>(type)) { return 4; }
	if (dynamic_cast<const Real_type*>(type)) { return 3; }
	if (dynamic_cast<const Long_type*>(type)) { return 2; }
	if (dynamic_cast<const Integer_type*>(type)) { return 1; }
	if (dynamic_cast<const Bool_type*>(type)) { return 0; }
	return -1;
}
llvm::Type* Bool_type::llvm() const {
	return llvm::Type::getInt1Ty(Compiler::context);
}
std::string Bool_type::clazz() const {
	return "Boolean";
}
std::ostream& Bool_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "bool" << END_COLOR;
	return os;
}

}