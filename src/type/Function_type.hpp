#ifndef FUNCTION_TYPE_HPP
#define FUNCTION_TYPE_HPP

#include "Base_type.hpp"
#include "Type.hpp"

namespace ls {

class Function_type : public Base_type {
	bool _closure;
	Type _return_type;
	std::vector<Type> _arguments;
public:
	Function_type(const Type&, const std::vector<Type>&, bool closure = false);
	bool closure() const { return _closure; }
	virtual const std::string getName() const { return "function"; }
	virtual const std::string getJsonName() const { return "function"; }
	virtual bool operator == (const Base_type*) const override;
	virtual bool compatible(const Base_type*) const override;
	virtual Type return_type() const override;
	virtual std::vector<Type> arguments() const override;
	virtual Type argument(size_t) const override;
	virtual llvm::Type* llvm() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif