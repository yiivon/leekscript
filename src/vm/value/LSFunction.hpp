#ifndef LSFUNCTION_H_
#define LSFUNCTION_H_

#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include "../LSValue.hpp"
#include <map>
#include <jit/jit.h>
#include "../../../lib/json.hpp"
#include "../Type.hpp"

namespace ls {

template <class T = LSValue*>
class LSFunction : public LSValue {
public:

	static LSClass* function_class;

	void* function;
	std::vector<LSValue*> captures;

	LSFunction(void* function);
	virtual ~LSFunction();

	void add_capture(LSValue* value);
	LSValue* get_capture(int index);

	// For reflexion
	std::vector<LSValue*> args;
	LSValue* return_type;

	/*
	 * LSValue methods
	 */
	LSVALUE_OPERATORS
	bool isTrue() const override;
	bool eq(const LSFunction<LSValue*>*) const override;
	bool lt(const LSFunction<LSValue*>*) const override;
	LSValue* attr(const std::string& key) const override;
	std::ostream& dump(std::ostream& os) const;
	std::string json() const;
	LSValue* getClass() const override;
	int typeID() const override { return 8; }
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSFunction.tcc"
#endif

#endif
