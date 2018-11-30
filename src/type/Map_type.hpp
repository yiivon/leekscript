#ifndef MAP_TYPE_HPP
#define MAP_TYPE_HPP

#include "Type.hpp"
#include "BaseRawType.hpp"

namespace ls {

class Map_type : public BaseRawType {
	Type _key;
	Type _element;
public:
	Map_type(Type key, Type element) : _key(key), _element(element) {}
	virtual int id() const { return 6; }
	virtual const std::string getName() const { return "map"; }
	virtual const std::string getClass() const { return "Map"; }
	virtual const std::string getJsonName() const { return "map"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual Type element() const override;
	virtual Type key() const override;
	virtual bool operator == (const BaseRawType*) const override;
	virtual bool compatible(const BaseRawType*) const override;
	virtual llvm::Type* llvm() const override;
};

}

#endif