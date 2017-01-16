#ifndef LSOBJECT_HPP_
#define LSOBJECT_HPP_

#include "../LSValue.hpp"
#include "LSClass.hpp"
#include "../../../lib/json.hpp"
#include "../Type.hpp"

namespace ls {

class LSObject : public LSValue {
public:
	static LSValue* object_class;

	std::map<std::string, LSValue*> values;
	LSClass* clazz;
	bool readonly;

	LSObject();
	LSObject(LSClass*);
	virtual ~LSObject();

	/** LSObject methods **/
	void addField(std::string name, LSValue* value);
	LSArray<LSValue*>* ls_get_keys() const;
	LSArray<LSValue*>* ls_get_values() const;
	LSObject* ls_map(LSFunction<LSValue*>* fun) const;

	/** LSValue methods **/
	LSVALUE_OPERATORS
	bool isTrue() const override;
	bool eq(const LSObject*) const override;
	bool lt(const LSObject*) const override;
	bool in(const LSValue*) const override;
	LSValue* attr(const std::string& key) const override;
	LSValue** attrL(const std::string& key) override;
	LSValue* abso() const override;
	LSValue* clone() const override;
	std::ostream& dump(std::ostream& os) const override;
	std::string json() const override;
	LSValue* getClass() const override;
	int typeID() const override { return 9; }
};

}

#endif
