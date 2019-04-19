#include "Functions.hpp"
#include "../value/LSArray.hpp"
#include "../value/LSString.hpp"
#include "../VM.hpp"

namespace ls {
namespace legacy {

void Functions::add(VM* vm, std::string name, Callable callable) {
	vm->add_internal_var(name, callable.versions[0].type, nullptr, new Callable(callable));
}

void Functions::create(VM* vm) {
	add(vm, "count", {
		{"?", Type::fun(Type::integer(), {Type::any()}), (void*) &Functions::v1_count}
	});
	add(vm, "charAt", {
		{"?", Type::fun(Type::string(), {Type::string(), Type::integer()}), (void*) &Functions::v1_charAt}
	});
	add(vm, "inArray", {
		{"?", Type::fun(Type::boolean(), {Type::array(Type::any()), Type::any()}), (void*) &Functions::v1_inArray}
	});
	add(vm, "push", {
		{"?", Type::fun({}, {Type::array(Type::any()), Type::any()}), (void*) &Functions::v1_push}
	});
	add(vm, "pushAll", {
		{"?", Type::fun({}, {Type::array(Type::any()), Type::array(Type::any())}), (void*) &Functions::v1_pushAll}
	});
	add(vm, "replace", {
		{"?", Type::fun(Type::string(), {Type::string(), Type::string(), Type::string()}), (void*) &Functions::v1_replace}
	});
	add(vm, "fill", {
		{"?", Type::fun({}, {Type::array(), Type::any()}), (void*) &Functions::v1_fill},
		{"?", Type::fun({}, {Type::array(), Type::any(), Type::integer()}), (void*) &Functions::v1_fill_2},
	});
}

LSValue* Functions::v1_charAt(LSString* v, int p) {
	auto s = v->charAt(p);
	LSValue::delete_temporary(v);
	return s;
}

int Functions::v1_count(LSArray<LSValue*>* a) {
	int s = a->size();
	LSValue::delete_temporary(a);
	return s;
}

bool Functions::v1_inArray(LSArray<LSValue*>* a, LSValue* b) {
	return a->ls_contains(b);
}

LSValue* Functions::v1_push(LSArray<LSValue*>* a, LSValue* b) {
	return a->ls_push(b);
}

LSValue* Functions::v1_pushAll(LSArray<LSValue*>* a, LSArray<LSValue*>* b) {
	return a->ls_push_all_ptr(b);
}

void Functions::v1_fill(LSArray<LSValue*>* a, LSValue* b) {
	a->ls_fill(b, a->size());
}

void Functions::v1_fill_2(LSArray<LSValue*>* a, LSValue* b, int c) {
	a->ls_fill(b, c);
}

}
}
