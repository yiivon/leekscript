#include "Interval_type.hpp"
#include "../colors.h"
#include "Type.hpp"

namespace ls {

Type Interval_type::element() const {
	return Type::INTEGER;
}
llvm::Type* Interval_type::llvm() const {
	return Type::LLVM_LSVALUE_TYPE_PTR;
}

}