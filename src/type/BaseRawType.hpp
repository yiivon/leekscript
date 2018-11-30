#ifndef BASE_RAW_TYPE
#define BASE_RAW_TYPE

#include <string>

namespace ls {

class Type;

class BaseRawType {
public:
	virtual ~BaseRawType() = 0;
	virtual int id() const { return 0; }
	virtual const std::string getName() const = 0;
	virtual const std::string getClass() const { return "?"; }
	virtual const std::string getJsonName() const { return "?"; }
	virtual bool iterable() const { return false; }
	virtual bool is_container() const { return false; }
	virtual int size() const { return 64; }
	virtual bool is_placeholder() const { return false; }
	virtual Type element() const;
	virtual Type key() const;
	virtual bool operator == (const BaseRawType*) const;
	virtual bool compatible(const BaseRawType*) const;
	virtual std::ostream& print(std::ostream&) const;
};

std::ostream& operator << (std::ostream&, const BaseRawType*);

}

#endif