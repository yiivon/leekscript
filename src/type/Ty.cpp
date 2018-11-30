#include "Ty.hpp"
#include "Any_type.hpp"
#include "Bool_type.hpp"
#include "Number_type.hpp"
#include "Integer_type.hpp"
#include "Real_type.hpp"
#include "Int_type.hpp"
#include "Long_type.hpp"
#include "Double_type.hpp"
#include "Mpz_type.hpp"
#include "Char_type.hpp"
#include "List_type.hpp"
#include "Array_type.hpp"
#include "String_type.hpp"
#include "Function_type.hpp"
#include "Interval_type.hpp"
#include "Class_type.hpp"
#include "Object_type.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>

namespace ls {

int Ty::id_generator = 0;
int Ty::primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151 };
std::map<int, std::shared_ptr<Base_type>> Ty::type_map;

std::shared_ptr<Base_type> Ty::any_type;
std::shared_ptr<Base_type> Ty::int_type;
std::shared_ptr<Base_type> Ty::bool_type;
std::shared_ptr<Base_type> Ty::integer_type;
std::shared_ptr<Base_type> Ty::real_type;
std::shared_ptr<Base_type> Ty::double_type;
std::shared_ptr<Base_type> Ty::long_type;
std::shared_ptr<Base_type> Ty::number_type;
std::shared_ptr<Base_type> Ty::mpz_type;
std::shared_ptr<Base_type> Ty::string_type;
std::shared_ptr<Base_type> Ty::class_type;
std::shared_ptr<Base_type> Ty::object_type;
std::shared_ptr<Base_type> Ty::interval_type;
std::shared_ptr<Base_type> Ty::fun_type;
std::shared_ptr<Base_type> Ty::char_type;

void Ty::init_types() {
	Ty::any_type = std::make_shared<Base_type>(Any_type());
	Ty::int_type = std::make_shared<Base_type>(Int_type());
	Ty::bool_type = std::make_shared<Base_type>(Bool_type());
	Ty::integer_type = std::make_shared<Base_type>(Integer_type());
	Ty::real_type = std::make_shared<Base_type>(Real_type());
	Ty::double_type = std::make_shared<Base_type>(Double_type());
	Ty::long_type = std::make_shared<Base_type>(Long_type());
	Ty::number_type = std::make_shared<Base_type>(Number_type());
	Ty::mpz_type = std::make_shared<Base_type>(Mpz_type());
	Ty::string_type = std::make_shared<Base_type>(String_type());
	Ty::class_type = std::make_shared<Base_type>(Class_type());
	Ty::object_type = std::make_shared<Base_type>(Object_type());
	Ty::fun_type = std::make_shared<Base_type>(Function_type());
	Ty::char_type = std::make_shared<Base_type>(Char_type());

	auto add = [](std::shared_ptr<Base_type> type) {
		type_map.emplace(type->id(), type);
	};
	// add(Any_type());
	// add(Number_type());
	// add(Integer_type());
	// add(Int_type());
	// add(Long_type());
	// add(Real_type());
	// add(Double_type());
	// add(String_type());
	// add(Char_type());
	// add(List_type());
	// add(Array_type());
	add(any_type);
	add(int_type);
	add(bool_type);
}

Ty::Ty() {}
Ty::Ty(std::shared_ptr<const Base_type> type, bool constant) : constant(constant) {
	add(type);
}

Ty::~Ty() {
	// std::cout << "destroy type " << *this << std::endl;
}

bool Ty::operator == (const Ty& type) const {
	return _types.size() == type._types.size() && std::equal(_types.begin(), _types.end(), type._types.begin(), [](std::shared_ptr<const Base_type> a, std::shared_ptr<const Base_type> b) {
		return *a == *b;
	});
}

Ty Ty::get_compatible(const Ty& type) const {
	if (_types.size() == 0) return type;
	if (type._types.size() == 0) return *this;

	const List_type* list1, *list2;
	if ((list1 = dynamic_cast<const List_type*>(_types[0].get())) && (list2 = dynamic_cast<const List_type*>(type._types[0].get()))) {
		return { std::make_shared<List_type>(list1->element().get_compatible(list2->element())) };
	}
	auto id1 = _types[0]->id();
	auto id2 = type._types[0]->id();
	return { type_map.at(std::gcd(id1, id2)) };
}

void Ty::add(const Ty type) {
	for (const auto& t : type._types) {
		add(t);
	}
}

void Ty::add(const std::shared_ptr<const Base_type> type) {
	for (const auto& t : _types) {
		if (*t == *type) return;
	}
	_types.push_back(type);
}

Ty Ty::merge(const Ty& type) const {
	auto new_type = Ty::get_void();
	for (const auto& t : _types) {
		new_type.add(t);
	}
	for (const auto& t : type._types) {
		new_type.add(t);
	}
	return new_type;
}

bool Ty::all(std::function<bool(const Base_type* const)> fun) const {
	return std::all_of(begin(_types), end(_types), [&](auto& type) {
		return fun(type.get());
	});
}

bool Ty::iterable() const {
	return std::all_of(begin(_types), end(_types), [](auto& type) {
		return type->iterable();	
	});
}

bool Ty::container() const {
	return std::all_of(begin(_types), end(_types), [](auto& type) {
		return type->container();	
	});
}

Ty Ty::element() const {
	if (_types.size() == 0) return {};
	return { _types[0]->element() };
}
Ty Ty::key() const {
	return {}; // TODO
}
std::vector<Ty> Ty::arguments() const {
	return {}; // TODO
}
Ty Ty::argument(int) const {
	return {}; // TODO
}
Ty Ty::get_return() const {
	return Ty::any();
}

int Ty::get_next_id() {
	return primes[id_generator++];
}
Ty Ty::get_void() {
	return { };
}
Ty Ty::any() {
	return { any_type };
}
Ty Ty::get_bool() {
	return { bool_type };
}
Ty Ty::const_bool() {
	return { bool_type, true };
}
Ty Ty::get_number() {
	return { number_type };
}
Ty Ty::get_integer() {
	return { integer_type };
}
Ty Ty::real() {
	return { real_type };
}
Ty Ty::get_int() {
	return { int_type };
}
Ty Ty::get_long() {
	return { long_type };
}
Ty Ty::get_double() {
	return { double_type };
}
Ty Ty::mpz() {
	return { mpz_type };
}
Ty Ty::get_string() {
	return { string_type };
}
Ty Ty::get_class() {
	return { class_type };
}
Ty Ty::object() {
	return { object_type };
}
Ty Ty::interval() {
	return { interval_type };
}
Ty Ty::fun() {
	return { fun_type };
}
Ty Ty::get_char() {
	return { char_type };
}

std::ostream& operator << (std::ostream& os, const Ty& ty) {
	if (ty._types.size() == 0) {
		os << "(void)";
	} else if (ty._types.size() == 1) {
		os << *ty._types.begin();
	} else {
		for (auto i = ty._types.begin(); i != ty._types.end(); ++i) {
			if (i != ty._types.begin()) os << " | ";
			os << *i;
		}
	}
	return os;
}

}