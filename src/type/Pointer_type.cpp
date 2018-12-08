#include "Pointer_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

Pointer_type::Pointer_type(Type type) : _type(type) {
	_llvm_type = type.llvm_type()->getPointerTo();
}
bool Pointer_type::operator == (const Base_type* type) const {
	if (auto p = dynamic_cast<const Pointer_type*>(type)) {
		return p->_type == _type;
	}
	return false;
}
llvm::Type* Pointer_type::llvm() const {
	return _llvm_type;
}
std::ostream& Pointer_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "pointer" << END_COLOR;
	return os;
}

}