#include "Meta_add_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

bool Meta_add_type::operator == (const Type* type) const {
	return false;
}
int Meta_add_type::distance(const Type* type) const {
	return -1;
}
llvm::Type* Meta_add_type::llvm(const Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
std::string Meta_add_type::class_name() const {
	return "";
}
std::ostream& Meta_add_type::print(std::ostream& os) const {
	os << C_GREY << t1 << " | " << t2 << END_COLOR;
	return os;
}
Type* Meta_add_type::clone() const {
	return new Meta_add_type(t1, t2);
}

}