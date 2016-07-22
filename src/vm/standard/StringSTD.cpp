#include <sstream>
#include <vector>
#include <math.h>
#include <string.h>
#include "../../../lib/utf8.h"

#include "StringSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSArray.hpp"

using namespace std;

namespace ls {

StringSTD::StringSTD() : Module("String") {

	method("charAt", Type::STRING, Type::STRING, {Type::INTEGER_P}, (void*) &LSString::charAt);
	method("contains", Type::STRING, Type::BOOLEAN_P, {Type::STRING}, (void*) &string_contains);
	method("endsWith", Type::STRING, Type::BOOLEAN_P, {Type::STRING}, (void*) &string_endsWith);
	method("indexOf", Type::STRING, Type::INTEGER, {Type::STRING}, (void*) &string_indexOf);
	method("length", Type::STRING, Type::INTEGER_P, {}, (void*) &string_length);
	method("size", Type::STRING, Type::INTEGER_P, {}, (void*) &string_size);
	method("replace", Type::STRING, Type::STRING, {Type::STRING, Type::STRING}, (void*) &string_replace);
	method("reverse", Type::STRING, Type::STRING, {}, (void*) &string_reverse);
	method("substring", Type::STRING, Type::STRING, {Type::INTEGER_P, Type::INTEGER_P}, (void*) &string_substring);
	method("toArray", Type::STRING, Type::ARRAY, {}, (void*) &string_toArray);
	method("toLower", Type::STRING, Type::STRING, {}, (void*) &string_toLower);
	method("toUpper", Type::STRING, Type::STRING, {}, (void*) &string_toUpper);
	method("split", Type::STRING, Type::STRING_ARRAY, {Type::STRING}, (void*) &string_split);
	method("startsWith", Type::STRING, Type::BOOLEAN_P, {Type::STRING}, (void*) &string_startsWith);
	method("code", {
		{Type::STRING, Type::INTEGER, {}, (void*) &string_begin_code},
		{Type::STRING, Type::INTEGER, {Type::INTEGER}, (void*) &string_code},
	});
	method("number", Type::STRING, Type::LONG, {}, (void*) &string_number);

	Type map_fun_type = Type::FUNCTION;
	map_fun_type.setArgumentType(0, Type::STRING);
	map_fun_type.setReturnType(Type::STRING);
	method("map", Type::STRING, Type::STRING, {map_fun_type}, (void*) &string_map);

	/*
	 * Static methods
	 */
	static_method("charAt", Type::STRING, {Type::STRING, Type::INTEGER_P}, (void*) &string_charAt);
	static_method("contains", Type::BOOLEAN_P, {Type::STRING, Type::STRING}, (void*) &string_contains);
	static_method("endsWith", Type::BOOLEAN_P, {Type::STRING, Type::STRING}, (void*) &string_endsWith);
	static_method("length", Type::INTEGER_P, {Type::STRING}, (void*) &string_length);
	static_method("size", Type::INTEGER_P, {Type::STRING}, (void*) &string_size);
	static_method("replace", Type::STRING, {Type::STRING, Type::STRING, Type::STRING}, (void*) &string_replace);
	static_method("reverse", Type::STRING, {Type::STRING}, (void*) &string_reverse);
	static_method("substring", Type::STRING, {Type::STRING, Type::INTEGER_P, Type::INTEGER_P}, (void*) &string_substring);
	static_method("toArray", Type::ARRAY, {Type::STRING}, (void*) &string_toArray);
	static_method("toLower", Type::STRING, {Type::STRING}, (void*) &string_toLower);
	static_method("toUpper", Type::STRING, {Type::STRING}, (void*) &string_toUpper);
	static_method("split", Type::STRING_ARRAY, {Type::STRING, Type::STRING}, (void*) &string_split);
	static_method("startsWith", Type::BOOLEAN_P, {Type::STRING, Type::STRING}, (void*) &string_startsWith);
	static_method("map", Type::STRING, {Type::STRING, map_fun_type}, (void*) &string_map);
	static_method("code", {
		{Type::INTEGER, {Type::STRING}, (void*) &string_begin_code},
		{Type::INTEGER, {Type::STRING, Type::INTEGER}, (void*) &string_code},
	});
	static_method("number", Type::LONG, {Type::STRING}, (void*) &string_number);
}

StringSTD::~StringSTD() {}

LSValue* string_charAt(LSString* string, LSNumber* index) {
	return new LSString(string->operator[] (index->value));
}

LSValue* string_contains(LSString* haystack, LSString* needle) {
	return LSBoolean::get(haystack->find(*needle) != string::npos);
}

LSValue* string_endsWith(LSString* string, LSString* ending) {
	if (ending->size() > string->size()) {
		return LSBoolean::false_val;
	}
	return LSBoolean::get(std::equal(ending->rbegin(), ending->rend(), string->rbegin()));
}

int string_indexOf(LSString* string, LSString* needle) {
	if (needle->size() > string->size()) {
		return -1;
	}
	return string->find(*needle);
}

LSValue* string_length(LSString* string) {
	return new LSNumber(string->size());
}

LSValue* string_map(const LSString* s, void* function) {

	char buff[5];
	std::string new_string = string("");
	auto fun = (void* (*)(void*)) function;

	const char* string_chars = s->c_str();
	int i = 0;
	int l = strlen(string_chars);

	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);
		u8_toutf8(buff, 5, &c, 1);
		LSString* ch = new LSString(buff);
		ch->refs = 1;
		LSString* res = (LSString*) fun(ch);
		new_string += *res;
		LSValue::delete_val(res);
	}
	return new LSString(new_string);
}

LSValue* string_replace(LSString* string, LSString* from, LSString* to) {
	std::string str(*string);
	size_t start_pos = 0;
	while((start_pos = str.find(*from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from->length(), *to);
		start_pos += to->length();
	}
	return new LSString(str);
}

LSValue* string_reverse(LSString* string) {
	return string->operator ~();
}

LSValue* string_size(LSString* string) {
	return LSNumber::get(string->unicode_length());
}

LSValue* string_split(LSString* string, LSString* delimiter) {
	LSArray<LSString*>* parts = new LSArray<LSString*>();
	if (*delimiter == "") {
		for (char c : *string) {
			parts->push_no_clone(new LSString(c));
		}
		return parts;
	} else {
		size_t last = 0;
		size_t pos = 0;
		while ((pos = string->find(*delimiter, last)) != std::wstring::npos) {
			parts->push_no_clone(new LSString(string->substr(last, pos - last)));
			last = pos + delimiter->size();
		}
		parts->push_no_clone(new LSString(string->substr(last)));
		return parts;
	}
}

LSValue* string_startsWith(const LSString* string, const LSString* starting) {
	if (starting->size() > string->size()) {
		return LSBoolean::false_val;
	}
	return LSBoolean::get(std::equal(starting->begin(), starting->end(), string->begin()));
}

LSValue* string_substring(LSString* string, LSNumber* start, LSNumber* length) {
	return new LSString(string->substr(start->value, length->value));
}

LSValue* string_toArray(const LSString* string) {
	LSArray<LSValue*>* parts = new LSArray<LSValue*>();
	for (char c : *string) {
		parts->push_no_clone(new LSString(c));
	}
	return parts;
}

LSValue* string_toLower(LSString* s) {
	string new_s = string(*s);
	for (auto& c : new_s) c = tolower(c);
	return new LSString(new_s);
}

LSValue* string_toUpper(LSString* s) {
	string new_s = string(*s);
	for (auto& c : new_s) c = toupper(c);
	return new LSString(new_s);
}

int string_begin_code(const LSString* v) {
	return LSString::u8_char_at((char*) v->c_str(), 0);
}

int string_code(const LSString* v, int pos) {
	return LSString::u8_char_at((char*) v->c_str(), pos);
}

long string_number(const LSString* s) {
	return stol(*s);
}

}
