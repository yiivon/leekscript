#ifndef LONG_TYPE_HPP
#define LONG_TYPE_HPP

#include "Integer_type.hpp"

namespace ls {

class Long_type : public Integer_type {
	static int ID;
public:
	Long_type(int id = 1, const std::string name = "long");
	virtual ~Long_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif