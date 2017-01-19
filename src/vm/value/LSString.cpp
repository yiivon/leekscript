#include <sstream>
#include <algorithm>
#include "LSString.hpp"
#include "LSNull.hpp"
#include "LSBoolean.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"
#include <string.h>
#include "../../../lib/utf8.h"

using namespace std;

namespace ls {

LSValue* LSString::string_class(new LSClass("String"));

LSString::iterator LSString::iterator_begin(LSString* s) {
	return {(char*) s->c_str(), 0, 0, 0, 0};
}

void LSString::iterator_next(LSString::iterator* it) {
	if (it->pos == it->next_pos) {
		u8_nextchar((char*) it->buffer, &it->pos);
		it->next_pos = it->pos;
	} else {
		it->pos = it->next_pos;
	}
	it->index++;
}

u_int32_t LSString::iterator_get(LSString::iterator* it) {
	if (it->pos == it->next_pos) {
		int pos = it->pos;
		u_int32_t c = u8_nextchar(it->buffer, &pos);
		it->next_pos = pos;
		it->character = c;
	}
	return it->character;
}

int LSString::iterator_key(LSString::iterator* it) {
	return it->index;
}

bool LSString::iterator_end(LSString::iterator* it) {
	bool end = it->buffer[it->pos] == 0;
	return end;
}

LSString::LSString() {}
LSString::LSString(const char value) : string(string(1, value)) {}
LSString::LSString(const char* value) : string(value) {}
LSString::LSString(const string& value) : string(value) {}
LSString::LSString(const Json& json) : string(json.get<std::string>()) {}

LSString::~LSString() {}

LSString* LSString::charAt(int index) const {
	return new LSString(this->operator[] (index));
}

int LSString::unicode_length() const {
	return u8_strlen(this->c_str());
}

bool LSString::is_permutation(LSString* other) {
	bool result = this->size() == other->size() and std::is_permutation(this->begin(), this->end(), other->begin());
	LSValue::delete_temporary(this);
	LSValue::delete_temporary(other);
	return result;
}

LSString* LSString::sort() {
	std::string res = *this;
	std::sort(res.begin(), res.end());
	LSValue::delete_temporary(this); // TODO move if tmp
	return new LSString(res);
}

bool LSString::is_palindrome() const {
	bool r = std::equal(this->begin(), this->begin() + this->size() / 2, this->rbegin());
	LSValue::delete_temporary(this);
	return r;
}

LSValue* LSString::ls_foldLeft(LSFunction<LSValue*>* function, LSValue* v0) {
	char buff[5];
	auto fun = (LSValue* (*)(void*, LSValue*, LSValue*)) function->function;
	const char* string_chars = this->c_str();
	int i = 0;
	int l = strlen(string_chars);
	LSValue* result = ls::move(v0);
	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);
		u8_toutf8(buff, 5, &c, 1);
		LSString* ch = new LSString(buff);
		ch->refs = 1;
		result = fun(function, result, ch);
		LSValue::delete_ref(ch);
	}
	LSValue::delete_temporary(this);
	return result;
}

/*
 * LSValue methods
 */
bool LSString::isTrue() const {
	return size() > 0;
}

LSValue* LSString::ls_not() {
	bool r = size() == 0;
	if (refs == 0) delete this;
	return LSBoolean::get(r);
}

LSValue* LSString::ls_tilde() {
	char buff[5];
	char* string_chars = (char*) this->c_str();
	int i = 0;
	int l = strlen(string_chars);
	string reversed = "";
	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);
		u8_toutf8(buff, 5, &c, 1);
		reversed = buff + reversed;
	}
	if (refs == 0) delete this;
	return new LSString(reversed);
}

LSValue* LSString::ls_add(LSNull*) {
	if (refs == 0) {
		this->append("null");
		return this;
	}
	return new LSString(*this + "null");
}

LSValue* LSString::ls_add(LSBoolean* boolean) {
	if (refs == 0) {
		this->append(boolean->value ? "true" : "false");
		return this;
	}
	return new LSString(*this + (boolean->value ? "true" : "false"));
}

LSValue* LSString::ls_add(LSNumber* value) {
	if (refs == 0) {
		this->append(value->toString());
		if (value->refs == 0) delete value;
		return this;
	}
	LSValue* r = new LSString(*this + value->toString());
	if (value->refs == 0) delete value;
	return r;
}

LSValue* LSString::ls_add(LSString* string) {
	if (refs == 0) {
		this->append(*string);
		if (string->refs == 0) delete string;
		return this;
	}
	LSValue* r = new LSString(*this + *string);
	if (string->refs == 0) delete string;
	return r;
}

LSValue* LSString::ls_add(LSArray<LSValue*>* array) {
	if (refs == 0) {
		this->append("<array>");
		if (array->refs == 0) delete array;
		return this;
	}
	LSValue* r = new LSString(*this + "<array>");
	if (array->refs == 0) delete array;
	return r;
}

LSValue* LSString::ls_add(LSArray<int>* array) {
	if (refs == 0) {
		this->append("<array>");
		if (array->refs == 0) delete array;
		return this;
	}
	LSValue* r = new LSString(*this + "<array>");
	if (array->refs == 0) delete array;
	return r;
}

LSValue* LSString::ls_add(LSObject* object) {
	if (refs == 0) {
		this->append("<object>");
		if (object->refs == 0) delete object;
		return this;
	}
	LSValue* r = new LSString(*this + "<object>");
	if (object->refs == 0) delete object;
	return r;
}

LSValue* LSString::ls_add(LSFunction<LSValue*>* function) {
	if (refs == 0) {
		this->append("<function>");
		if (function->refs == 0) delete function;
		return this;
	}
	LSValue* r = new LSString(*this + "<function>");
	if (function->refs == 0) delete function;
	return r;
}

LSValue* LSString::ls_add(LSClass*) {
	if (refs == 0) {
		this->append("<class>");
		return this;
	}
	LSValue* r = new LSString(*this + "<class>");
	return r;
}

LSValue* LSString::ls_add_eq(LSNull*) {
	append("null");
	return this;
}
LSValue* LSString::ls_add_eq(LSBoolean* boolean) {
	append(boolean->value ? "true" : "false");
	return this;
}
LSValue* LSString::ls_add_eq(LSNumber* value) {
	append(value->toString());
	if (value->refs == 0) delete value;
	return this;
}
LSValue* LSString::ls_add_eq(LSString* string) {
	append(*string);
	if (string->refs == 0) delete string;
	return this;
}
LSValue* LSString::ls_add_eq(LSArray<LSValue*>* array) {
	append("<array>");
	if (array->refs == 0) delete array;
	return this;
}
LSValue* LSString::ls_add_eq(LSArray<int>* array) {
	append("<array>");
	if (array->refs == 0) delete array;
	return this;
}
LSValue* LSString::ls_add_eq(LSObject* object) {
	append("<object>");
	if (object->refs == 0) delete object;
	return this;
}
LSValue* LSString::ls_add_eq(LSFunction<LSValue*>* function) {
	append("<function>");
	LSValue::delete_temporary(function);
	return this;
}
LSValue* LSString::ls_add_eq(LSClass*) {
	append("<class>");
	return this;
}

LSValue* LSString::ls_mul(LSNumber* number) {
	string r;
	for (int i = 0; i < number->value; ++i) {
		r += *this;
	}
	if (number->refs == 0) delete number;
	if (refs == 0) {
		*this = r;
		return this;
	}
	return new LSString(r);
}

LSValue* LSString::ls_div(LSString* s) {

	char buff[5];
	char* string_chars = (char*) this->c_str();

	LSArray<LSValue*>* array = new LSArray<LSValue*>();

	if (s->size() == 0) {
		int i = 0;
		int l = strlen(string_chars);
		while (i < l) {
			u_int32_t c = u8_nextchar(string_chars, &i);
			u8_toutf8(buff, 5, &c, 1);
			LSString* ch = new LSString(buff);
			array->push_inc(ch);
		}
 	} else {

 		u_int32_t separator = u8_char_at((char*) s->c_str(), 0);
		int i = 0;
		int l = strlen(string_chars);
		std::string item = "";
		while (i < l) {
			u_int32_t c = u8_nextchar(string_chars, &i);
			if (c == separator) {
				array->push_inc(new LSString(item));
				item = "";
			} else {
				u8_toutf8(buff, 5, &c, 1);
				item += buff;
			}
		}
		array->push_inc(new LSString(item));
//		stringstream ss(*this);
//		string item;
//		while (getline(ss, item, s->operator[] (0))) {
//			array->push_no_clone(new LSString(item));
//		}
 	}
	if (s->refs == 0) delete s;
	if (refs == 0) delete this;
	return array;
}

bool LSString::eq(const LSString* str) const {
	return compare(*str) == 0;
}

bool LSString::lt(const LSString* str) const {
	return compare(*str) < 0;
}


u_int32_t LSString::u8_char_at(char* s, int pos) {
	int i = 0;
	int j = 0;
	int l = strlen(s);
	while (i < l) {
		u_int32_t c = u8_nextchar(s, &i);
		if (j++ == pos) return c;
	}
	return 0;
}

LSValue* LSString::at(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
//		return new LSString(this->operator[] ((int) n->value));
		char buff[5];
		u_int32_t c = u8_char_at((char*) this->c_str(), (int) n->value);
		u8_toutf8(buff, 5, &c, 1);
		return new LSString(buff);
	}
	return LSNull::get();
}

LSValue** LSString::atL(const LSValue*) {
	// TODO
	return nullptr;
}

LSValue* LSString::range(int start, int end) const {

	char buff[5];
	char* string_chars = (char*) this->c_str();
	std::string new_string;

	int i = 0;
	int j = 0;
	int l = strlen(string_chars);
	while (i < l) {
		j++;
		u_int32_t c = u8_nextchar(string_chars, &i);
		if (j <= start) continue;
		u8_toutf8(buff, 5, &c, 1);
		new_string += buff;
		if (j > end) break;
	}
	return new LSString(new_string);
//	return new LSString(this->substr(start, end - start + 1));
}
LSValue* LSString::rangeL(int, int) {
	// TODO
	return this;
}

LSValue* LSString::abso() const {
	return LSNumber::get(unicode_length());
}

std::ostream& LSString::print(std::ostream& os) const {
	os << (std::string) *this;
	return os;
}

std::ostream& LSString::dump(std::ostream& os) const {
	os << "'" << escape_control_characters() << "'";
	return os;
}

string LSString::json() const {
	return "\"" + escaped('"') + "\"";
}

string LSString::escape_control_characters() const {
	std::string res = *this;
	res.erase(std::remove(res.begin(), res.end(), '\b'), res.end());
	res.erase(std::remove(res.begin(), res.end(), '\f'), res.end());
	res.erase(std::remove(res.begin(), res.end(), '\r'), res.end());
	return res;
}

string LSString::escaped(char quote) const {

	char buff[5];
	char* string_chars = (char*) this->c_str();
	std::string new_string;
	int i = 0;
	int l = strlen(string_chars);
	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);

		if (c == '\b') {
			// don't print the backspace
		}
		/* else if (c == '\f') {
			new_string += "\\f";
		} else if (c == '\n') {
			new_string += "\\n";
		} else if (c == '\r') {
			new_string += "\\r";
		} else if (c == '\t') {
			new_string += "\\t";
		} else
		*/
		else if (c == (u_int32_t)quote) {
			new_string += '\\';
			new_string += quote;
		} else {
			u8_toutf8(buff, 5, &c, 1);
			new_string += buff;
		}
	}
	return new_string;
}

LSValue* LSString::clone() const {
	return new LSString((std::string) *this);
}

std::ostream& operator << (std::ostream& os, const LSString& obj) {
	os << obj;
	return os;
}

LSValue* LSString::getClass() const {
	return LSString::string_class;
}

}
