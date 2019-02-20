#include "Mpz_type.hpp"
#include "Type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"
#include "Number_type.hpp"
#include "Any_type.hpp"
#include "Real_type.hpp"
#include "Mpz_type.hpp"
#include "Bool_type.hpp"
#include "Integer_type.hpp"
#include "Long_type.hpp"

namespace ls {

llvm::Type* Mpz_type::mpz_type = nullptr;

bool Mpz_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Mpz_type*>(type) != nullptr;
}
int Mpz_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 2; }
	if (dynamic_cast<const Number_type*>(type)) { return 1; }
	if (dynamic_cast<const Mpz_type*>(type)) { return 0; }
	if (dynamic_cast<const Real_type*>(type)) { return 100; }
	if (dynamic_cast<const Long_type*>(type)) { return 200; }
	if (dynamic_cast<const Integer_type*>(type)) { return 300; }
	if (dynamic_cast<const Bool_type*>(type)) { return 400; }
	return -1;
}
llvm::Type* Mpz_type::llvm(const Compiler& c) const {
	return get_mpz_type(c);
}
std::string Mpz_type::clazz() const {
	return "Number";
}
std::ostream& Mpz_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "mpz" << END_COLOR;
	return os;
}

llvm::Type* Mpz_type::get_mpz_type(const Compiler& c) {
	if (mpz_type == nullptr) {
		// llvm::StructType::create({llvm::Type::getInt32Ty(Compiler::context), llvm::Type::getInt32Ty(Compiler::context), llvm::Type::getInt32PtrTy(Compiler::context)}, "mpz", true);
		mpz_type = llvm::Type::getInt128Ty(c.getContext());
	}
	return mpz_type;
}

}