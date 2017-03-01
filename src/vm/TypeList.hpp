#ifndef TYPE_LIST_HPP
#define TYPE_LIST_HPP

#include <set>
#include "Type.hpp"

namespace ls {

struct TypeList : public std::vector<Type> {

	TypeList& operator = (const Type& type);

	void add(TypeList& list);
	void add(const Type& type);

	bool will_take(const std::vector<Type>& args_types);

	Type getReturnType() const;
	void setReturnType(Type type);

	Nature nature() const;
	void setNature(Nature nature);
	const BaseRawType* raw_type() const;
};

std::ostream& operator << (std::ostream&, const TypeList&);

}

#endif
