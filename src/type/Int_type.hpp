#ifndef INT_TYPE_HPP
#define INT_TYPE_HPP

#include "Long_type.hpp"

namespace ls {

class Int_type : public Long_type {
	static int _id;
public:
	Int_type();
	virtual ~Int_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif