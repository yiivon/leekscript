#ifndef REAL_TYPE_HPP
#define REAL_TYPE_HPP

#include "Number_type.hpp"

namespace ls {

class FloatRawType : public NumberRawType {
public:
	FloatRawType() {}
	virtual const std::string getName() const { return "real"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
};

class Real_type : public Number_type {
	static int _id;
public:
	Real_type(int id = 1, const std::string = "real");
	virtual ~Real_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif