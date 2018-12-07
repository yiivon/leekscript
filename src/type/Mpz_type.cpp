#include "Mpz_type.hpp"
#include "Type.hpp"
#include "../colors.h"
#include "../compiler/LLVMCompiler.hpp"

namespace ls {

llvm::Type* Mpz_type::mpz_type = nullptr;

bool Mpz_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Mpz_type*>(type);
}
llvm::Type* Mpz_type::llvm() const {
	return get_mpz_type();
}
std::ostream& Mpz_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "mpz" << END_COLOR;
	return os;
}

llvm::Type* Mpz_type::get_mpz_type() {
	if (mpz_type == nullptr) {
		// llvm::StructType::create({llvm::Type::getInt32Ty(LLVMCompiler::context), llvm::Type::getInt32Ty(LLVMCompiler::context), llvm::Type::getInt32PtrTy(LLVMCompiler::context)}, "mpz", true);
		mpz_type = llvm::Type::getInt128Ty(LLVMCompiler::context);
	}
	return mpz_type;
}

}