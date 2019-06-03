#ifndef FUNCTION_TYPE_HPP
#define FUNCTION_TYPE_HPP

#include "Pointer_type.hpp"
#include "Type.hpp"
#include "../compiler/value/Value.hpp"

namespace ls {

class Function_type : public Pointer_type {
	const Type* const _return_type;
	std::vector<const Type*> _arguments;
	bool _closure;
	const Value* _function;
public:
	Function_type(const Type*, const std::vector<const Type*>&, bool closure = false, const Value* function = nullptr);
	bool closure() const { return _closure; }
	const Value* function() const { return _function; }
	virtual int id() const override { return 9; }
	virtual const std::string getName() const { return "function"; }
	virtual const std::string getJsonName() const { return "function"; }
	virtual bool callable() const override { return true; }
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual bool castable(const Base_type*) const override;
	virtual const Type* return_type() const override;
	virtual const std::vector<const Type*>& arguments() const override;
	virtual const Type* argument(size_t) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;

	static llvm::Type* function_type;
	static llvm::Type* get_function_type();
};

}

#endif