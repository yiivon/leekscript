#ifndef CLASS_TYPE_HPP
#define CLASS_TYPE_HPP

#include "BaseRawType.hpp"
#include "Base_type.hpp"

namespace ls {

class ClassRawType : public BaseRawType {
public:
	ClassRawType() {}
	virtual int id() const { return 11; }
	virtual const std::string getName() const { return "class"; }
	virtual const std::string getClass() const { return "Class"; }
	virtual const std::string getJsonName() const { return "class"; }
	virtual llvm::Type* llvm() const override;
};

class Class_type : public Base_type {
public:
	Class_type();
	virtual ~Class_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif