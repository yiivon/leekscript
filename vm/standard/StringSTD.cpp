#include <sstream>
#include <vector>
#include <math.h>

#include "StringSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSArray.hpp"

using namespace std;

namespace ls {

StringSTD::StringSTD() : Module("String") {

	method("charAt", Type::STRING, Type::STRING, {Type::INTEGER_P}, (void*) &LSString::charAt);
	method("contains", Type::STRING, Type::BOOLEAN_P, {Type::STRING}, (void*) &string_contains);
	method("endsWith", Type::STRING, Type::BOOLEAN_P, {Type::STRING}, (void*) &string_endsWith);
	method("length", Type::STRING, Type::INTEGER_P, {}, (void*) &string_length);
	method("size", Type::STRING, Type::INTEGER_P, {}, (void*) &string_size);
	method("replace", Type::STRING, Type::STRING, {Type::STRING, Type::STRING}, (void*) &string_replace);
	method("reverse", Type::STRING, Type::STRING, {}, (void*) &string_reverse);
	method("substring", Type::STRING, Type::STRING, {Type::INTEGER_P, Type::INTEGER_P}, (void*) &string_substring);
	method("toArray", Type::STRING, Type::ARRAY, {}, (void*) &string_toArray);
	method("toLower", Type::STRING, Type::STRING, {}, (void*) &string_toLower);
	method("toUpper", Type::STRING, Type::STRING, {}, (void*) &string_toUpper);
	method("split", Type::STRING, Type::STRING, {Type::STRING}, (void*) &string_split);
	method("startsWith", Type::STRING, Type::BOOLEAN_P, {Type::STRING}, (void*) &string_startsWith);

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
	static_method("split", Type::STRING, {Type::STRING, Type::STRING}, (void*) &string_split);
	static_method("startsWith", Type::BOOLEAN_P, {Type::STRING, Type::STRING}, (void*) &string_startsWith);
	static_method("map", Type::STRING, {Type::STRING, map_fun_type}, (void*) &string_map);
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

LSValue* string_indexOf(LSString* haystack, LSString* needle) {
	return LSNumber::get(haystack->find(*needle));
}

LSValue* string_length(LSString* string) {
	return new LSNumber(string->size());
}

LSValue* string_map(const LSString* s, void* function) {
	std::string new_string = string("");
	auto fun = (void* (*)(void*)) function;
	for (char v : *s) {
		new_string += *((LSString*) fun(new LSString(v)));
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
	return new LSNumber(string->size());
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
		while ((pos = string->find(*delimiter, last)) != std::string::npos) {
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

}
