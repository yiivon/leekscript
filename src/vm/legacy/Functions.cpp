#include "Functions.hpp"
#include "../value/LSArray.hpp"
#include "../value/LSString.hpp"
#include "../VM.hpp"

namespace ls {
namespace legacy {

void Functions::add(VM* vm, std::string name, Type return_type, std::vector<Type> args, void* fun) {
	auto f = new LSFunction(fun);
	f->native = true;
	auto type = Type::fun(return_type, args);
	type.native = true;
	vm->add_internal_var(name, type, f);
}

void Functions::create(VM* vm) {
	add(vm, "debug", {}, {Type::any()}, (void*) &Functions::v1_debug);
	add(vm, "count", Type::integer(), {Type::any()}, (void*) &Functions::v1_count);
	add(vm, "charAt", Type::string(), {Type::string(), Type::integer()}, (void*) &Functions::v1_charAt);
	add(vm, "pushAll", {}, {Type::array(Type::any()), Type::array(Type::any())}, (void*) &Functions::v1_pushAll);
	add(vm, "replace", Type::string(), {Type::string(), Type::string(), Type::string()}, (void*) &Functions::v1_replace);
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

LSValue* Functions::v1_pushAll(LSArray<LSValue*>* a, LSArray<LSValue*>* b) {
	return a->ls_push_all_ptr(b);
}

}
}
