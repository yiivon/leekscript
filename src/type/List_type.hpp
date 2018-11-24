#ifndef LIST_TYPE_HPP
#define LIST_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class List_type : public Base_type {
private:
	std::shared_ptr<Base_type> element_type;
public:
	List_type(std::shared_ptr<Base_type> element) : element_type(element) {}
	virtual ~List_type();

	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	std::shared_ptr<Base_type> element() const;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif