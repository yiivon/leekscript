#ifndef ARRAY_TYPE_HPP
#define ARRAY_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Array_type : public Base_type {
private:
	std::shared_ptr<Base_type> element_type;
public:
	Array_type();
	Array_type(std::shared_ptr<Base_type> element);
	virtual ~Array_type();

	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	std::shared_ptr<Base_type> element() const;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif