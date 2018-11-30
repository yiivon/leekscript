#ifndef FUNCTION_TYPE_HPP
#define FUNCTION_TYPE_HPP

#include "Base_type.hpp"
#include "BaseRawType.hpp"

namespace ls {

class FunctionRawType : public BaseRawType {
public:
	FunctionRawType() {}
	virtual int id() const { return 9; }
	virtual const std::string getName() const { return "function"; }
	virtual const std::string getClass() const { return "Function"; }
	virtual const std::string getJsonName() const { return "function"; }
	virtual llvm::Type* llvm() const override;
};

class ClosureRawType : public FunctionRawType {
public:
	ClosureRawType() {}
	virtual int id() const { return 12; }
};

class Function_type : public Base_type {
public:
	Function_type();
	virtual ~Function_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif