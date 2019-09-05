#include "Template_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"
#include "../compiler/Compiler.hpp"

namespace ls {

void Template_type::reset() const {
	((Template_type*) this)->_implementation = Type::void_;
}
void Template_type::implement(const Type* implementation) const {
	((Template_type*) this)->_implementation = implementation;
}
bool Template_type::operator == (const Type* type) const {
	if (this == type) return true;
	if (_implementation == Type::void_) return false;
	return _implementation->operator == (type);
}
llvm::Type* Template_type::llvm(const Compiler& c) const {
	// assert(_implementation._types.size() > 0);
	// return _implementation._types[0]->llvm();
	return llvm::Type::getInt32Ty(c.getContext());
}
std::ostream& Template_type::print(std::ostream& os) const {
	os << BLUE_BOLD << _name;
	if (_implementation != Type::void_) {
		os << "." << _implementation;
	}
	os << END_COLOR;
	return os;
}
Type* Template_type::clone() const {
	return new Template_type { _name };
}

}