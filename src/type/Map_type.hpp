#ifndef MAP_TYPE_HPP
#define MAP_TYPE_HPP

#include "Type.hpp"
#include "Pointer_type.hpp"

namespace ls {

class Map_type : public Pointer_type {
	const Type* const _key;
	const Type* const _element;
public:
	Map_type(const Type* key, const Type* element);
	virtual int id() const override { return 6; }
	virtual const std::string getName() const { return "map"; }
	virtual const std::string getJsonName() const { return "map"; }
	virtual bool iterable() const { return true; }
	virtual const Type* iterator() const override;
	virtual bool is_container() const { return true; }
	virtual const Type* element() const override;
	virtual const Type* key() const override;
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type*) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream& os) const override;
};

}

#endif