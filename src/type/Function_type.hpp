#ifndef FUNCTION_TYPE_HPP
#define FUNCTION_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Function_type : public Base_type {
public:
	Function_type();
	virtual ~Function_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif