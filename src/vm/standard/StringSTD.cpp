#include <sstream>
#include <vector>
#include <math.h>
#include <string.h>
#include "../../../lib/utf8.h"
#include "StringSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSArray.hpp"
#include "../VM.hpp"

namespace ls {

LSValue* string_charAt(LSString* string, int index);
bool string_contains(LSString* haystack, LSString* needle);
bool string_endsWith(LSString* string, LSString* ending);
int string_indexOf(LSString* haystack, LSString* needle);
int string_length(LSString* string);
LSArray<LSValue*>* string_split(LSString* string, LSString* delimiter);
bool string_startsWith(const LSString* string, const LSString* starting);
LSValue* string_substring(LSString* string, int start, int length);
LSValue* string_toLower(LSString* string);
LSValue* string_toUpper(LSString* string);
LSValue* string_toArray(const LSString* string);
int string_begin_code(const LSString*);
LSValue* string_begin_code_ptr(const LSString*);
int string_code(const LSString*, int pos);
long string_number(const LSString*);

LSString* plus_any(LSString* s, LSValue* v) {
	return (LSString*) s->add(v);
}

LSString* plus_mpz(LSString* s, __mpz_struct* mpz) {
	char buff[1000];
	mpz_get_str(buff, 10, mpz);
	LSString* res = new LSString(*s + buff);
	LSValue::delete_temporary(s);
	return res;
}

LSString* plus_mpz_tmp(LSString* s, __mpz_struct* mpz) {
	char buff[1000];
	mpz_get_str(buff, 10, mpz);
	LSString* res = new LSString(*s + buff);
	LSValue::delete_temporary(s);
	mpz_clear(mpz);
	VM::current()->mpz_deleted++;
	return res;
}

StringSTD::StringSTD() : Module("String") {

	LSString::string_class = clazz;

	/*
	 * Constructor
	 */
	constructor_({
		{Type::tmp_string(), {Type::i8().pointer()}, (void*) &LSString::constructor, Method::NATIVE}
	});

	/*
	 * Operators
	 */
	operator_("+", {
		{Type::string(), Type::any(), Type::string(), (void*) &plus_any, {}, Method::NATIVE},
		{Type::string(), Type::mpz_ptr(), Type::string(), (void*) &plus_mpz, {}, Method::NATIVE},
		{Type::string(), Type::tmp_mpz_ptr(), Type::string(), (void*) &plus_mpz_tmp, {}, Method::NATIVE},
		{Type::string(), Type::real(), Type::string(), (void*) &StringSTD::add_real, {}, Method::NATIVE},
		{Type::string(), Type::integer(), Type::string(), (void*) &StringSTD::add_int, {}, Method::NATIVE},
		{Type::string(), Type::boolean(), Type::string(), (void*) &StringSTD::add_bool, {}, Method::NATIVE},
	});
	operator_("<", {
		{Type::string(), Type::string(), Type::boolean(), (void*) &StringSTD::lt}
	});
	operator_("/", {
		{Type::string(), Type::string(), Type::array(Type::string()), (void*) &StringSTD::div}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::string(), {Type::const_string()}, (void*) &ValueSTD::copy}
	});
	method("charAt", {
		{Type::string(), {Type::const_string(), Type::const_integer()}, (void*) &string_charAt, Method::NATIVE},
	});
	method("contains", {
		{Type::boolean(), {Type::const_string(), Type::const_string()}, (void*) &string_contains, Method::NATIVE},
	});
	method("endsWith", {
		{Type::boolean(), {Type::const_string(), Type::const_string()}, (void*) &string_endsWith, Method::NATIVE},
	});
	Type fold_fun_type = Type::fun(Type::any(), {Type::any(), Type::string()});
	Type fold_clo_type = Type::closure(Type::any(), {Type::any(), Type::string()});
	method("fold", {
		{Type::any(), {Type::const_string(), fold_fun_type, Type::any()}, (void*) fold_fun},
		{Type::any(), {Type::const_string(), fold_clo_type, Type::any()}, (void*) fold_fun},
	});
	method("indexOf", {
		{Type::integer(), {Type::const_string(), Type::const_string()}, (void*) &string_indexOf, Method::NATIVE},
	});
	method("isPermutation", {
		{Type::boolean(), {Type::const_string(), Type::const_any()}, (void*) &LSString::is_permutation, Method::NATIVE},
	});
	method("isPalindrome", {
		{Type::boolean(), {Type::const_string()}, (void*) &LSString::is_palindrome, Method::NATIVE},
	});
	method("length", {
		{Type::integer(), {Type::const_string()}, (void*) &string_length, Method::NATIVE},
	});
	method("lines", {
		{Type::tmp_array(Type::string()), {Type::const_string()}, (void*) &LSString::ls_lines, Method::NATIVE},
	});
	method("size", {
		{Type::any(), {Type::const_string()}, (void*) &LSString::ls_size_ptr, Method::NATIVE},
		{Type::integer(), {Type::const_string()}, (void*) &LSString::ls_size, Method::NATIVE},
	});
	method("replace", {
		{Type::string(), {Type::const_string(), Type::const_string(), Type::const_string()}, (void*) &StringSTD::replace, Method::NATIVE},
		{Type::string(), {Type::const_string(), Type::const_string(), Type::const_string()}, (void*) &StringSTD::v1_replace, Method::NATIVE, {}, true},
	});
	method("reverse", {
		{Type::string(), {Type::const_string()}, (void*) &LSString::ls_tilde, Method::NATIVE},
	});
	method("substring", {
		{Type::string(), {Type::const_string(), Type::const_integer(), Type::const_integer()}, (void*) &string_substring, Method::NATIVE},
	});
	method("toArray", {
		{Type::array(Type::any()), {Type::const_string()}, (void*) &string_toArray, Method::NATIVE},
	});
	method("toLower", {
		{Type::string(), {Type::const_string()}, (void*) &string_toLower, Method::NATIVE},
	});
	method("toUpper", {
		{Type::string(), {Type::const_string()}, (void*) &string_toUpper, Method::NATIVE},
	});
	method("split", {
		{Type::tmp_array(Type::string()), {Type::const_string(), Type::const_string()}, (void*) &string_split, Method::NATIVE},
		{Type::tmp_array(Type::string()), {Type::const_any(), Type::const_any()}, (void*) &string_split, Method::NATIVE},
	});
	method("startsWith", {
		{Type::boolean(), {Type::const_string(), Type::const_string()}, (void*) &string_startsWith, Method::NATIVE},
	});
	method("code", {
		{Type::any(), {Type::const_any()}, (void*) &string_begin_code_ptr, Method::NATIVE},
		{Type::integer(), {Type::const_string()}, (void*) &string_begin_code, Method::NATIVE},
		{Type::integer(), {Type::const_any()}, (void*) &string_begin_code, Method::NATIVE},
		{Type::integer(), {Type::const_string(), Type::const_integer()}, (void*) &string_code, Method::NATIVE},
	});
	method("number", {
		{Type::long_(), {Type::const_string()}, (void*) &string_number, Method::NATIVE},
		{Type::long_(), {Type::const_any()}, (void*) &string_number, Method::NATIVE},
	});
	auto map_fun = &LSString::ls_map<LSFunction*>;
	method("map", {
		{Type::string(), {Type::const_string(), Type::fun(Type::string(), {Type::string()})}, (void*) map_fun, Method::NATIVE},
	});
	method("sort", {
		{Type::string(), {Type::const_string()}, (void*) &LSString::sort, Method::NATIVE},
	});
	method("wordCount", {
		{Type::any(), {Type::const_string()}, (void*) &LSString::word_count_ptr, Method::NATIVE},
		{Type::integer(), {Type::const_string()}, (void*) &LSString::word_count, Method::NATIVE},
	});

	/** Internal **/
	method("to_bool", {
		{Type::boolean(), {Type::const_string()}, (void*) &LSString::to_bool, Method::NATIVE}
	});
	method("codePointAt", {
		{Type::tmp_string(), {Type::const_string(), Type::integer()}, (void*) &LSString::codePointAt, Method::NATIVE}
	});
	method("isize", {
		{Type::string(), {Type::const_string()}, (void*) &LSString::int_size, Method::NATIVE}
	});
}

StringSTD::~StringSTD() {}

/*
 * Operators
 */
LSString* StringSTD::add_int(LSString* s, int i) {
	if (s->refs == 0) {
		s->append(std::to_string(i));
		return s;
	} else {
		return new LSString(*s + std::to_string(i));
	}
}

LSString* StringSTD::add_bool(LSString* s, bool b) {
	if (s->refs == 0) {
		s->append(b ? "true" : "false");
		return s;
	} else {
		return new LSString(*s + (b ? "true" : "false"));
	}
}

LSString* StringSTD::add_real(LSString* s, double i) {
	if (s->refs == 0) {
		s->append(LSNumber::print(i));
		return s;
	} else {
		return new LSString(*s + LSNumber::print(i));
	}
}

Compiler::value StringSTD::lt(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_call(Type::boolean(), args, +[](LSValue* a, LSValue* b) {
		return a->lt(b);
	});
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value StringSTD::div(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::array(Type::string()), args, +[](LSValue* a, LSValue* b) {
		return a->div(b);
	});
}

/*
 * Methods
 */
LSValue* string_charAt(LSString* string, int index) {
	LSValue* r = new LSString(string->operator[] (index));
	LSValue::delete_temporary(string);
	return r;
}

bool string_contains(LSString* haystack, LSString* needle) {
	bool r = haystack->find(*needle) != std::string::npos;
	LSValue::delete_temporary(haystack);
	LSValue::delete_temporary(needle);
	return r;
}

bool string_endsWith(LSString* string, LSString* ending) {
	if (ending->size() > string->size()) {
		LSValue::delete_temporary(string);
		LSValue::delete_temporary(ending);
		return false;
	}
	bool r = std::equal(ending->rbegin(), ending->rend(), string->rbegin());
	LSValue::delete_temporary(string);
	LSValue::delete_temporary(ending);
	return r;
}

int string_indexOf(LSString* string, LSString* needle) {
	int pos = -1;
	if (needle->size() <= string->size()) {
		pos = string->find(*needle);
	}
	LSValue::delete_temporary(string);
	LSValue::delete_temporary(needle);
	return pos;
}

int string_length(LSString* string) {
	int r = string->size();
	LSValue::delete_temporary(string);
	return r;
}

LSString* StringSTD::replace(LSString* string, LSString* from, LSString* to) {
	std::string str(*string);
	size_t start_pos = 0;
	while ((start_pos = str.find(*from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from->length(), *to);
		start_pos += to->length();
	}
	if (string->refs == 0) { delete string; }
	if (from->refs == 0) { delete from; }
	if (to->refs == 0) { delete to; }
	return new LSString(str);
}

LSValue* StringSTD::v1_replace(LSString* string, LSString* from, LSString* to) {
	std::string str(*string);
	size_t start_pos = 0;
	// Replace \\ by \ (like Java does)
	std::string f = *from;
	while ((start_pos = f.find("\\\\", start_pos)) != std::string::npos) {
		f.replace(start_pos, 2, "\\");
		start_pos += 1;
	}
	start_pos = 0;
	std::string t = *to;
	while ((start_pos = t.find("\\\\", start_pos)) != std::string::npos) {
		t.replace(start_pos, 2, "\\");
		start_pos += 1;
	}
	start_pos = 0;
	while ((start_pos = str.find(f, start_pos)) != std::string::npos) {
		str.replace(start_pos, from->length(), t);
		start_pos += t.size();
	}
	if (string->refs == 0) { delete string; }
	if (from->refs == 0) { delete from; }
	if (to->refs == 0) { delete to; }
	return new LSString(str);
}

LSArray<LSValue*>* string_split(LSString* string, LSString* delimiter) {
	auto parts = new LSArray<LSValue*>();
	if (*delimiter == "") {
		for (char c : *string) {
			parts->push_inc(new LSString(c));
		}
		if (string->refs == 0) {
			delete string;
		}
		if (delimiter->refs == 0) {
			delete delimiter;
		}
		return parts;
	} else {
		size_t last = 0;
		size_t pos = 0;
		while ((pos = string->find(*delimiter, last)) != std::wstring::npos) {
			parts->push_inc(new LSString(string->substr(last, pos - last)));
			last = pos + delimiter->size();
		}
		parts->push_inc(new LSString(string->substr(last)));
		if (string->refs == 0) {
			delete string;
		}
		if (delimiter->refs == 0) {
			delete delimiter;
		}
		return parts;
	}
}

bool string_startsWith(const LSString* string, const LSString* starting) {
	if (starting->size() > string->size()) {
		LSValue::delete_temporary(string);
		LSValue::delete_temporary(starting);
		return false;
	}
	bool r = std::equal(starting->begin(), starting->end(), string->begin());
	LSValue::delete_temporary(string);
	LSValue::delete_temporary(starting);
	return r;
}

LSValue* string_substring(LSString* string, int start, int length) {
	LSValue* r = new LSString(string->substr(start, length));
	if (string->refs == 0) {
		delete string;
	}
	return r;
}

LSValue* string_toArray(const LSString* string) {
	LSArray<LSValue*>* parts = new LSArray<LSValue*>();
	for (char c : *string) {
		parts->push_inc(new LSString(c));
	}
	if (string->refs == 0) {
		delete string;
	}
	return parts;
}

LSValue* string_toLower(LSString* s) {
	std::string new_s = std::string(*s);
	for (auto& c : new_s) c = tolower(c);
	if (s->refs == 0) {
		delete s;
	}
	return new LSString(new_s);
}

LSValue* string_toUpper(LSString* s) {
	std::string new_s = std::string(*s);
	for (auto& c : new_s) c = toupper(c);
	if (s->refs == 0) {
		delete s;
	}
	return new LSString(new_s);
}

int string_begin_code(const LSString* v) {
	int r = LSString::u8_char_at((char*) v->c_str(), 0);
	LSValue::delete_temporary(v);
	return r;
}

LSValue* string_begin_code_ptr(const LSString* v) {
	auto code = string_begin_code(v);
	auto r = LSNumber::get(code);
	return r;
}

int string_code(const LSString* v, int pos) {
	int r = LSString::u8_char_at((char*) v->c_str(), pos);
	LSValue::delete_temporary(v);
	return r;
}

long string_number(const LSString* s) {
	long r = stol(*s);
	if (s->refs == 0) delete s;
	return r;
}

Compiler::value StringSTD::fold_fun(Compiler& c, std::vector<Compiler::value> args) {
	auto function = args[1];
	auto result = c.create_and_add_var("r", args[2].t);
	c.insn_store(result, c.insn_move_inc(args[2]));
	c.insn_foreach(args[0], {}, "v", "", [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto r = c.insn_call(function.t.return_type(), {c.insn_load(result), v}, function);
		c.insn_delete(c.insn_load(result));
		c.insn_store(result, c.insn_move_inc(r));
		return {};
	});
	return c.insn_load(result);
}

}
