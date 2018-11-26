#ifndef NUMBER_TYPE_HPP
#define NUMBER_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Number_type : public Base_type {
	static int _id;
public:
	Number_type(int id = 1, const std::string name = "number");
	virtual ~Number_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif