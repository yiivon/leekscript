#include "Meta_element_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

bool Meta_element_type::operator == (const Type* type) const {
	return this->type == type;
}
int Meta_element_type::distance(const Type* type) const {
	return 0;
}
llvm::Type* Meta_element_type::llvm(const Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
std::string Meta_element_type::class_name() const {
	return "";
}
std::ostream& Meta_element_type::print(std::ostream& os) const {
	os << type << ".element";
	return os;
}
Type* Meta_element_type::clone() const {
	return new Meta_element_type(type);
}

}