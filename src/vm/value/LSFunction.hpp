#ifndef LSFUNCTION_H_
#define LSFUNCTION_H_

#include <iostream>
#include <vector>
#include <memory>
#include "../LSValue.hpp"

namespace ls {

template <class R, class ...A> R call(LSFunction* function, A... args);

class LSFunction : public LSValue {
public:

	static LSClass* clazz;
	static LSFunction* constructor(void* f);

	void* function;
	// For reflexion
	std::vector<LSValue*> args;
	LSValue* return_type;

	LSFunction(void* function);
	virtual ~LSFunction();
	virtual bool closure() const;

	LSValue* call(LSValue* object, LSValue* arg1) const {
		auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;
		return fun(object, arg1);
	}

	/*
	 * LSValue methods
	 */
	bool to_bool() const override;
	bool ls_not() const override;
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;
	LSValue* attr(const std::string& key) const override;
	LSValue* clone() const override;
	std::ostream& dump(std::ostream& os, int level) const override;
	std::string json() const override;
	LSValue* getClass() const override;
};

template <class R, class ...A> R call(LSFunction* function, A... args) {
	auto fun = (R (*)(A...)) function->function;
	return fun(args...);
}

}

#endif
