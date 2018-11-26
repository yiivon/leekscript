#ifndef LIST_TYPE_HPP
#define LIST_TYPE_HPP

#include "Base_type.hpp"
#include "Ty.hpp"

namespace ls {

class List_type : public Base_type {
	static int _id;
protected:
	std::shared_ptr<Ty> element_type;
public:
	List_type(std::shared_ptr<Ty> element = std::make_shared<Ty>(Ty::get_any()), int id = 1, const std::string name = "list");
	virtual ~List_type();

	std::shared_ptr<Ty> element() const;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif