#ifndef INTERVAL_TYPE_HPP
#define INTERVAL_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Interval_type : public Base_type {
public:
	Interval_type() {}
	virtual const std::string getName() const { return "interval"; }
	virtual const std::string getClass() const { return "Interval"; }
	virtual const std::string getJsonName() const { return "interval"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual Type element() const override;
	virtual llvm::Type* llvm() const override;
};

}

#endif