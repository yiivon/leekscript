#include "Char_type.hpp"
#include "Ty.hpp"
#include "../colors.h"

namespace ls {

int Char_type::_id = Ty::get_next_id();

Char_type::Char_type() : Base_type(_id, "char") {}
Char_type::~Char_type() {}

}