#ifndef NUMBER_TYPE_HPP
#define NUMBER_TYPE_HPP

#include "Base_type.hpp"
#include "BaseRawType.hpp"

namespace ls {

class NumberRawType : public BaseRawType {
public:
	NumberRawType() {}
	virtual int id() const { return 3; }
	virtual const std::string getName() const { return "number"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool iterable() const { return true; }
};

class Number_type : public Base_type {
	static int _id;
public:
	Number_type(int id = 1, const std::string name = "number");
	virtual ~Number_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif