#ifndef ANY_TYPE_HPP
#define ANY_TYPE_HPP

#include "Base_type.hpp"
#include "Base_type.hpp"

namespace ls {

class Any_type : public Base_type {
public:
	Any_type() {}
	virtual ~Any_type() {}
	virtual const std::string getName() const { return "any"; }
	virtual bool operator == (const Base_type*) const override;
	virtual llvm::Type* llvm() const;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif