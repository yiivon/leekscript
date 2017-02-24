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
	static LSClass* clazz;
	static LSNull* create() {
		return new LSNull();
	}
	static void set_null_value(LSNull* null_value) {
		LSNull::null_var = null_value;
	}

	~LSNull();

	bool isTrue() const override;
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;
	std::ostream& dump(std::ostream& os) const override;
	LSValue* getClass() const override;
};

}

#endif
