#ifndef META_ELEMENT_TYPE_HPP
#define META_ELEMENT_TYPE_HPP

#include "Any_type.hpp"

namespace ls {

class Meta_element_type : public Any_type {
public:
	const Type* type;
	Meta_element_type(const Type* type) : type(type) {
		placeholder = true;
	}
	virtual int id() const { return 0; }
	virtual const std::string getName() const override { return "meta_element"; }
	virtual const std::string getJsonName() const { return "meta_element"; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif