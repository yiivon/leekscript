#ifndef DOUBLE_TYPE_HPP
#define DOUBLE_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Double_type : public Base_type {
public:
	Double_type();
	virtual ~Double_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif