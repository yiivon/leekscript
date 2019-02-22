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
	add(vm, "debug", {
		{"?", Type::fun({}, {Type::any()}), (void*) &Functions::v1_debug}
	});
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

void Functions::v1_debug(LSValue* v) {
	v->print(VM::current()->output->stream());
	LSValue::delete_temporary(v);
	VM::current()->output->end();
}

LSValue* Functions::v1_charAt(LSString* v, int p) {
	auto s = v->charAt(p);
	LSValue::delete_temporary(v);
	return s;
}

LSValue* Functions::v1_replace(LSString* string, LSString* from, LSString* to) {
	std::string str(*string);
	size_t start_pos = 0;
	// Replace \\ by \ (like Java does)
	std::string f = *from;
	while((start_pos = f.find("\\\\", start_pos)) != std::string::npos) {
		f.replace(start_pos, 2, "\\");
		start_pos += 1;
	}
	start_pos = 0;
	std::string t = *to;
	while((start_pos = t.find("\\\\", start_pos)) != std::string::npos) {
		t.replace(start_pos, 2, "\\");
		start_pos += 1;
	}
	start_pos = 0;
	while((start_pos = str.find(f, start_pos)) != std::string::npos) {
		str.replace(start_pos, from->length(), t);
		start_pos += t.size();
	}
	if (string->refs == 0) { delete string; }
	if (from->refs == 0) { delete from; }
	if (to->refs == 0) { delete to; }
	return new LSString(str);
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
