#ifndef STRING_TYPE_HPP
#define STRING_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class String_type : public Base_type {
public:
	String_type();
	virtual ~String_type();

	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	std::shared_ptr<Base_type> element() const;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif