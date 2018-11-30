#ifndef STRING_TYPE_HPP
#define STRING_TYPE_HPP

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
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif