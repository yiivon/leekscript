#ifndef BOOL_TYPE_HPP
#define BOOL_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Bool_type : public Base_type {
	static int _id;
public:
	Bool_type();
	virtual ~Bool_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif