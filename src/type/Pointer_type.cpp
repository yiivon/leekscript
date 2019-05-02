#include "Pointer_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

Pointer_type::Pointer_type(Type type) : _type(type) {}

Type Pointer_type::pointed() const {
	return _type;
}
bool Pointer_type::operator == (const Base_type* type) const {
	if (auto p = dynamic_cast<const Pointer_type*>(type)) {
		return p->_type == _type;
	}
	return false;
}
int Pointer_type::distance(const Base_type* type) const {
	auto p = dynamic_cast<const Pointer_type*>(type);
	if (_type.is_mpz()) {
		if (p and p->pointed().is_mpz()) return 0;
		return _type._types[0]->distance(type);
	}
	if (p) {
		return _type.distance(p->_type);
	}
	return -1;
}
std::string Pointer_type::clazz() const {
	return _type.class_name();
}
llvm::Type* Pointer_type::llvm(const Compiler& c) const {
	if (_llvm_type == nullptr) {
		((Pointer_type*) this)->_llvm_type = _type.llvm_type(c)->getPointerTo();
	}
	return _llvm_type;
}
std::ostream& Pointer_type::print(std::ostream& os) const {
	os << _type << BLUE_BOLD << "*" << END_COLOR;
	return os;
}

}