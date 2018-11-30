#include "Mpz_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

llvm::Type* Mpz_type::llvm() const {
	return Type::LLVM_MPZ_TYPE;
}
std::ostream& Mpz_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "mpz" << END_COLOR;
	return os;
}

}