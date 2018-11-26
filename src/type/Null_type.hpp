#ifndef NULL_TYPE_HPP
#define NULL_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Null_type : public Base_type {
	static int _id;
public:
	Null_type();
	virtual ~Null_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif