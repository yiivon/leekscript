#include "LSFunction.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"

using namespace std;

namespace ls {

template <class R>
LSClass* LSFunction<R>::function_class = new LSClass("Function");

template <class R>
LSFunction<R>::LSFunction(void* function) {
	this->function = function;
	this->refs = 1;
	this->native = true;
}

template <class R>
LSFunction<R>::~LSFunction() {
	for (auto capture : captures) {
		LSValue::delete_ref(capture);
	}
}

template <class R>
void LSFunction<R>::add_capture(LSValue* value) {
	value->refs++;
	captures.push_back(value);
}

template <class R>
LSValue* LSFunction<R>::get_capture(int index) {
	return captures[index];
}

/*
 * LSValue methods
 */
template <class R>
bool LSFunction<R>::isTrue() const {
	return true;
}

template <class R>
bool LSFunction<R>::eq(const LSFunction<LSValue*>* f) const {
	return function == f->function;
}

template <class R>
bool LSFunction<R>::lt(const LSFunction<LSValue*>* f) const {
	return function < f->function;
}

template <class R>
LSValue* LSFunction<R>::attr(const std::string& key) const {
	if (key == "args") {
		LSArray<LSValue*>* args_list = new LSArray<LSValue*>();
		for (const auto& arg : args) {
			args_list->push_back(arg);
		}
		return args_list;
	}
	if (key == "return") {
		return return_type;
	}
	return LSNull::get();
}

template <class R>
std::ostream& LSFunction<R>::dump(std::ostream& os) const {
	os << "<function>";
	return os;
}

template <class R>
string LSFunction<R>::json() const {
	return ""; // don't output function in json
}

template <class R>
LSValue* LSFunction<R>::getClass() const {
	return LSFunction::function_class;
}

}
