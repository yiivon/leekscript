#include "Template_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"
#include "../compiler/Compiler.hpp"

namespace ls {

void Template_type::implement(const Type* implementation) const {
	((Template_type*) this)->_implementation = implementation;
}
bool Template_type::operator == (const Base_type* type) const {
	if (this == type) return true;
	if (_implementation == Type::void_) return false;
	return _implementation->_types[0]->operator == (type);
}
bool Template_type::compatible(const Base_type* type) const {
	if (_implementation == Type::void_) return false;
	return _implementation->_types[0]->compatible(type);
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

}