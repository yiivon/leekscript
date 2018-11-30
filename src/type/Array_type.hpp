#ifndef ARRAY_TYPE_HPP
#define ARRAY_TYPE_HPP

#include "Type.hpp"
#include "BaseRawType.hpp"

namespace ls {

class Array_type : public BaseRawType {
	Type _element;
public:
	Array_type(Type element) : _element(element) {}
	virtual int id() const { return 5; }
	virtual const std::string getName() const { return "array"; }
	virtual const std::string getClass() const { return "Array"; }
	virtual const std::string getJsonName() const { return "array"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual Type element() const override;
	virtual bool operator == (const BaseRawType*) const override;
	virtual bool compatible(const BaseRawType*) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

class Set_type : public BaseRawType {
	Type _element;
public:
	Set_type(Type element) : _element(element) {}
	virtual int id() const { return 7; }
	virtual const std::string getName() const { return "set"; }
	virtual const std::string getClass() const { return "Set"; }
	virtual const std::string getJsonName() const { return "set"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual Type element() const override;
	virtual bool operator == (const BaseRawType*) const override;
	virtual bool compatible(const BaseRawType*) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

class MapRawType : public BaseRawType {
public:
	MapRawType() {}
	virtual int id() const { return 6; }
	virtual const std::string getName() const { return "map"; }
	virtual const std::string getClass() const { return "Map"; }
	virtual const std::string getJsonName() const { return "map"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
};

}

#endif