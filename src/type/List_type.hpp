#ifndef LIST_TYPE_HPP
#define LIST_TYPE_HPP

#include "Base_type.hpp"
#include "Ty.hpp"

namespace ls {

class List_type : public Base_type {
	static int _id;
protected:
	Ty element_type;
public:
	List_type(Ty element = Ty::any(), int id = 1, const std::string name = "list");
	virtual ~List_type();

	Ty element() const;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif