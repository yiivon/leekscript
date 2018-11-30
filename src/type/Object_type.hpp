#ifndef OBJECT_TYPE_HPP
#define OBJECT_TYPE_HPP

#include "Base_type.hpp"
#include "BaseRawType.hpp"

namespace ls {

class ObjectRawType : public BaseRawType {
public:
	ObjectRawType() {}
	virtual int id() const { return 10; }
	virtual const std::string getName() const { return "object"; }
	virtual const std::string getClass() const { return "Object"; }
	virtual const std::string getJsonName() const { return "object"; }
	virtual bool iterable() const { return false; } // TODO not iterable for now
	virtual bool is_container() const { return true; }
	virtual llvm::Type* llvm() const override;
};

class Object_type : public Base_type {
public:
	Object_type();
	virtual ~Object_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif