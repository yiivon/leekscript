#ifndef INTERVAL_TYPE_HPP
#define INTERVAL_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Interval_type : public Base_type {
public:
	Interval_type() {}
	virtual int id() const override { return 8; }
	virtual const std::string getName() const { return "interval"; }
	virtual const std::string getJsonName() const { return "interval"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual Type element() const override;
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual std::string clazz() const override;
	virtual llvm::Type* llvm() const override;
};

}

#endif