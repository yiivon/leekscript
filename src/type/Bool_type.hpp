#ifndef BOOL_TYPE_HPP
#define BOOL_TYPE_HPP

#include "Base_type.hpp"
#include "BaseRawType.hpp"

namespace ls {

class BooleanRawType : public BaseRawType {
public:
	BooleanRawType() {}
	virtual int id() const { return 2; }
	virtual const std::string getName() const { return "bool"; }
	virtual const std::string getClass() const { return "Boolean"; }
	virtual const std::string getJsonName() const { return "boolean"; }
	virtual llvm::Type* llvm() const override;
};

class Bool_type : public Base_type {
	static int _id;
public:
	Bool_type();
	virtual ~Bool_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif