#ifndef META_BASEOF_TYPE_HPP
#define META_BASEOF_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Meta_baseof_type : public Type {
public:
	const Type* type;
	const Type* base;
	const Type* result = nullptr;
	Meta_baseof_type(const Type* type, const Type* base) : type(type), base(base) {}
	virtual void reset() const override;
	virtual int id() const { return 0; }
	virtual const std::string getName() const override { return "meta_baseof"; }
	virtual const std::string getJsonName() const { return "meta_baseof"; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual llvm::Type* llvm(const Compiler& c) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif