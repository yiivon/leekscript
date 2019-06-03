#ifndef PLACEHOLDER_TYPE_HPP
#define PLACEHOLDER_TYPE_HPP

#include "Any_type.hpp"
#include "Type.hpp"

namespace ls {

class Placeholder_type : public Any_type {
	std::string _name;
	const Type* _implementation = nullptr;
public:
	Placeholder_type(const std::string name) : _name(name) {}
	void implement(const Type* type) const;
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif