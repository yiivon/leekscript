#include "String_type.hpp"
#include "Char_type.hpp"
#include "Ty.hpp"
#include "../colors.h"
#include <iostream>
#include "Type.hpp"

namespace ls {

llvm::Type* StringRawType::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}

int String_type::_id = Ty::get_next_id();

String_type::String_type(int id, const std::string name) : List_type(Ty::get_char(), _id * id, name) {}
String_type::~String_type() {}

std::ostream& String_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "string" << END_COLOR;
	return os;
}

}