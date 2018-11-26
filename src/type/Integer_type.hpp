#ifndef INTEGER_TYPE_HPP
#define INTEGER_TYPE_HPP

#include "Number_type.hpp"

namespace ls {

class Integer_type : public Number_type {
	static int _id;
public:
	Integer_type(int id = 1, const std::string name = "integer");
	virtual ~Integer_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif