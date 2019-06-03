#ifndef SET_TYPE_HPP
#define SET_TYPE_HPP

#include "Type.hpp"
#include "Pointer_type.hpp"

namespace ls {

class Set_type : public Pointer_type {
	const Type* const _element;
public:
	Set_type(const Type* element);
	virtual int id() const override { return 7; }
	virtual const std::string getName() const { return "set"; }
	virtual const std::string getJsonName() const { return "set"; }
	virtual bool iterable() const { return true; }
	virtual const Type* iterator() const override;
	virtual bool is_container() const { return true; }
	virtual const Type* key() const override;
	virtual const Type* element() const override;
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream&) const override;

	static std::map<const Type*, std::shared_ptr<Set_type>> cache;
	static std::map<const Type*, const Type*> nodes;
	static std::map<const Type*, const Type*> iterators;
	static std::shared_ptr<Set_type> create(const Type* element);
	static const Type* get_node_type(const Type* element);
	static const Type* get_iterator(const Type* element);
};

}

#endif