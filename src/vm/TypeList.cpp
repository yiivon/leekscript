#include "TypeList.hpp"
#include <sstream>

namespace ls {

TypeList& TypeList::operator = (const Type& type) {
	clear();
	add(type);
	return *this;
}

void TypeList::add(TypeList& list) {
	for (auto& type : list) {
		add(type);
	}
}

void TypeList::add(const Type& type) {
	for (const auto& t : *this) {
		if (type == t)
			return;
	}
	push_back(type);
}

Type TypeList::getReturnType() const {
	return begin()->getReturnType();
}

void TypeList::setReturnType(Type) {
	//((Type*) begin())->setReturnType(type);
}

bool TypeList::will_take(const std::vector<Type>& args_types) {
	return begin()->will_take(args_types);
}

Nature TypeList::nature() const {
	return begin()->nature;
}

void TypeList::setNature(Nature nature) {
	begin()->nature = nature;
}

const BaseRawType* TypeList::raw_type() const {
	return begin()->raw_type;
}

std::ostream& operator << (std::ostream& os, const TypeList& type) {
	//if (type.size() == 1) {
		// os << *type.begin();
	// } else {
		os << "{";
		for (auto i = type.begin(); i != type.end(); ++i) {
			if (i != type.begin()) os << ", ";
			os << *i;
		}
		os << "}";
	// }
	return os;
}

}
