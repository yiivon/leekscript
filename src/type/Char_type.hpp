#ifndef CHAR_TYPE_HPP
#define CHAR_TYPE_HPP

#include "Base_type.hpp"

namespace ls {

class Char_type : public Base_type {
	static int _id;
public:
	Char_type();
	virtual ~Char_type();
};

}

#endif