#ifndef INTERVAL_TYPE_HPP
#define INTERVAL_TYPE_HPP

#include "BaseRawType.hpp"

namespace ls {

class Interval_type : public BaseRawType {
public:
	Interval_type() {}
	virtual int id() const { return 8; }
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