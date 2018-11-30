#ifndef NULL_TYPE_HPP
#define NULL_TYPE_HPP

#include "Base_type.hpp"
#include "BaseRawType.hpp"

namespace ls {

class NullRawType : public BaseRawType {
public:
	NullRawType() {}
	virtual int id() const { return 1; }
	virtual const std::string getName() const { return "null"; }
	virtual const std::string getClass() const { return "Null"; }
	virtual const std::string getJsonName() const { return "null"; }
};

class Null_type : public Base_type {
	static int _id;
public:
	Null_type();
	virtual ~Null_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif