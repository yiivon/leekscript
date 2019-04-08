#ifndef NEVER_TYPE_HPP
#define NEVER_TYPE_HPP

#include "Any_type.hpp"

namespace ls {

class Never_type : public Any_type {
public:
	Never_type();
	virtual ~Never_type() {}
	virtual const std::string getName() const { return "never"; }
	virtual bool operator == (const Base_type*) const override;
	virtual bool callable() const override { return true; }
	virtual int distance(const Base_type* type) const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif