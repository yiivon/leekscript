#ifndef INTEGER_TYPE_HPP
#define INTEGER_TYPE_HPP

#include "Number_type.hpp"

namespace ls {

class IntegerRawType : public NumberRawType {
public:
	IntegerRawType() {}
	virtual const std::string getName() const { return "int"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual int size() const { return 32; }
	virtual std::ostream& print(std::ostream&) const override;
};

class Integer_type : public Number_type {
	static int _id;
public:
	Integer_type(int id = 1, const std::string name = "integer");
	virtual ~Integer_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif