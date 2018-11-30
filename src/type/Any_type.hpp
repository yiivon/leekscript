#ifndef ANY_TYPE_HPP
#define ANY_TYPE_HPP

#include "Base_type.hpp"
#include "BaseRawType.hpp"

namespace ls {

class AnyRawType : public BaseRawType {
public:
	AnyRawType() {}
	virtual ~AnyRawType() {}
	virtual const std::string getName() const { return "any"; }
	virtual llvm::Type* llvm() const;
	virtual std::ostream& print(std::ostream& os) const override;
};

class PlaceholderRawType : public BaseRawType {
public:
	std::string name;
	PlaceholderRawType(std::string name) : name(name) {}
	virtual ~PlaceholderRawType() {}
	virtual int id() const { return -1; } // LCOV_EXCL_LINE id() method is used at compilation and placeholder types mustn't be compiled.
	virtual const std::string getName() const { return name; }
	virtual bool is_placeholder() const { return true; }
	virtual llvm::Type* llvm() const override { }
};

}

#endif