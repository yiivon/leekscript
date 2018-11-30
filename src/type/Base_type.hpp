#ifndef BASE_TYPE
#define BASE_TYPE

#include <string>
#include "llvm/IR/Type.h"

namespace ls {

class Type;

class Base_type {
public:
	virtual ~Base_type() = 0;
	virtual int id() const { return 0; }
	virtual const std::string getName() const = 0;
	virtual const std::string getClass() const { return "?"; }
	virtual const std::string getJsonName() const { return "?"; }
	virtual bool iterable() const { return false; }
	virtual bool is_container() const { return false; }
	virtual bool is_placeholder() const { return false; }
	virtual Type element() const;
	virtual Type key() const;
	virtual bool operator == (const Base_type*) const = 0;
	virtual bool compatible(const Base_type*) const;
	virtual Type iterator() const;
	virtual llvm::Type* llvm() const = 0;
	virtual std::ostream& print(std::ostream&) const;
};

std::ostream& operator << (std::ostream&, const Base_type*);

}

#endif