#ifndef STRING_TYPE_HPP
#define STRING_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class String_type : public Pointer_type {
public:
	String_type();
	virtual int id() const override { return 4; }
	virtual const std::string getName() const { return "string"; }
	virtual const std::string getJsonName() const { return "string"; }
	virtual bool iterable() const { return true; }
	virtual const Type* iterator() const override;
	virtual const Type* key() const override;
	virtual const Type* element() const override;
	virtual bool is_container() const { return true; }
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif