#include "BaseRawType.hpp"
#include "Type.hpp"

namespace ls {

BaseRawType::~BaseRawType() {}

Type BaseRawType::element() const {
	return {};
}
Type BaseRawType::key() const {
	return {};
}
bool BaseRawType::operator == (const BaseRawType*) const {
	return false;
}
bool BaseRawType::compatible(const BaseRawType*) const {
	return false;
}
Type BaseRawType::iterator() const {
	assert(false && "No iterator available on this type");
}
std::ostream& BaseRawType::print(std::ostream& os) const {
	os << "???";
	return os;
}

}