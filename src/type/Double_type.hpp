#ifndef DOUBLE_TYPE_HPP
#define DOUBLE_TYPE_HPP

#include "Real_type.hpp"

namespace ls {

class Double_type : public Real_type {
	static int _id;
public:
	Double_type();
	virtual ~Double_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif