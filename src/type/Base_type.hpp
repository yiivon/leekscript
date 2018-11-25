#ifndef BASE_TYPE_HPP
#define BASE_TYPE_HPP

#include <ostream>
#include <memory>
#include <string>
#include <functional>

namespace ls {

class Type;

class Base_type {
public:
	virtual ~Base_type();
	bool operator == (const Base_type&) const;
	virtual bool all(std::function<bool(const Base_type* const)>) const;
	virtual const std::string clazz() const { return "?"; }
	virtual const std::string json_name() const { return "?"; }
	virtual bool compatible(std::shared_ptr<Base_type>) const = 0;
	virtual bool iterable() const { return false; }
	virtual bool container() const { return false; }
	virtual int size() const { return 64; }
	virtual bool is_placeholder() const { return false; }
	virtual std::ostream& print(std::ostream&) const = 0;
};

std::ostream& operator << (std::ostream&, const Base_type*);

}

#endif