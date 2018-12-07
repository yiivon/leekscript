#ifndef ARRAY_TYPE_HPP
#define ARRAY_TYPE_HPP

#include "Type.hpp"
#include "Base_type.hpp"

namespace ls {

class Array_type : public Base_type {
	Type _element;
public:
	Array_type(Type element) : _element(element) {}
	virtual const std::string getName() const { return "array"; }
	virtual const std::string getJsonName() const { return "array"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual Type element() const override;
	virtual bool operator == (const Base_type*) const override;
	virtual bool compatible(const Base_type*) const override;
	virtual llvm::Type* llvm() const override;
	virtual Type iterator() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream&) const override;

	static llvm::Type* any_array_type;
	static llvm::Type* int_array_type;
	static llvm::Type* real_array_type;
	static llvm::Type* get_any_array_type();
	static llvm::Type* get_int_array_type();
	static llvm::Type* get_real_array_type();
};

}

#endif