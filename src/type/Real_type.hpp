#ifndef REAL_TYPE_HPP
#define REAL_TYPE_HPP

#include "Number_type.hpp"

namespace ls {

class Real_type : public Number_type {
public:
	Real_type() {}
	virtual const std::string getName() const { return "real"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual llvm::Type* llvm() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif