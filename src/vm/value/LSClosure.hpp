#ifndef LSCLOSURE_HPP
#define LSCLOSURE_HPP

#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <jit/jit.h>
#include "../LSValue.hpp"
#include "LSFunction.hpp"

namespace ls {

class LSClosure : public LSFunction {
public:

	std::vector<LSValue*> captures;
	std::vector<bool> captures_native;

	LSClosure(void* function);
	virtual ~LSClosure();
	virtual bool closure() const;

	void add_capture(LSValue* value);
	LSValue* get_capture(int index);
};

template <class R, class ...A> R call(LSFunction* function, A... args) {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	auto fun = (R (*)(A...)) function->function;
	return fun(args...);
}
template <class R, class ...A> R call(LSClosure* function, A... args) {
	auto fun = (R (*)(void*, A...)) function->function;
	return fun(function, args...);
}

}

#endif
