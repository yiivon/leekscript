#ifndef BOOL_TYPE_HPP
#define BOOL_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Bool_type : public Base_type {
public:
	Bool_type();
	virtual ~Bool_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif