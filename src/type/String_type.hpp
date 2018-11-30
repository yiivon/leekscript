#ifndef STRING_TYPE_HPP
#define STRING_TYPE_HPP

#include "List_type.hpp"
#include "BaseRawType.hpp"

namespace ls {

class StringRawType : public BaseRawType {
public:
	StringRawType() {}
	virtual int id() const { return 4; }
	virtual const std::string getName() const { return "string"; }
	virtual const std::string getClass() const { return "String"; }
	virtual const std::string getJsonName() const { return "string"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual llvm::Type* llvm() const override;
};

class String_type : public List_type {
	static int _id;
public:
	String_type(int id = 1, const std::string name = "string");
	virtual ~String_type();
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif