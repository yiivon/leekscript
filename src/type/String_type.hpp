#ifndef STRING_TYPE_HPP
#define STRING_TYPE_HPP

#include "List_type.hpp"

namespace ls {

class String_type : public List_type {
	static int _id;
public:
	String_type(int id = 1, const std::string name = "string");
	virtual ~String_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif