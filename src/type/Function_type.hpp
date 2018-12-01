#ifndef FUNCTION_TYPE_HPP
#define FUNCTION_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Function_type : public Base_type {
	bool _closure;
public:
	Function_type(bool closure = false) : _closure(closure) {}
	bool closure() const { return _closure; }
	virtual const std::string getName() const { return "function"; }
	virtual const std::string getJsonName() const { return "function"; }
	virtual bool operator == (const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif