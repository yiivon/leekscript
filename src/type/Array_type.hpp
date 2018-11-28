#ifndef ARRAY_TYPE_HPP
#define ARRAY_TYPE_HPP

#include "List_type.hpp"

namespace ls {

class Array_type : public List_type {
	static int _id;
public:
	Array_type(int id = 1);
	Array_type(Ty element, int id = 1);
	virtual ~Array_type();

	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif