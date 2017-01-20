#ifndef LSNULL
#define LSNULL

#include <iostream>
#include <string>
#include "../LSValue.hpp"
#include "../Type.hpp"

namespace ls {

class LSNull : public LSValue {
private:
	static LSValue* null_var;
	LSNull();

public:
	static LSValue* get();
	static LSClass* null_class;

	~LSNull();

	LSVALUE_OPERATORS
	bool isTrue() const override;
	bool eq(const LSNull*) const override;
	bool lt(const LSNull*) const override;
	std::ostream& dump(std::ostream& os) const override;
	LSValue* getClass() const override;
	int typeID() const override;
};

}

#endif
