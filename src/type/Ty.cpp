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
#include <iostream>
#include <algorithm>
#include <numeric>

namespace ls {

int Ty::id_generator = 0;
int Ty::primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151 };
std::map<int, std::shared_ptr<Base_type>> Ty::type_map;

void Ty::init_types() {
	auto add = [](Base_type type) {
		type_map.emplace(type.id(), std::make_shared<Base_type>(type));
	};
	add(Any_type());
	add(Number_type());
	add(Integer_type());
	add(Int_type());
	add(Long_type());
	add(Real_type());
	add(Double_type());
	add(String_type());
	add(Char_type());
	add(List_type());
	add(Array_type());
}

Ty::Ty() {}
Ty::Ty(std::shared_ptr<Base_type> type) {
	add(type);
}

Ty::~Ty() {}

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
		return { std::make_shared<List_type>(std::make_shared<Ty>(list1->element()->get_compatible(*list2->element()))) };
	}
	auto id1 = _types[0]->id();
	auto id2 = type._types[0]->id();
	return { type_map.at(std::gcd(id1, id2)) };
}

void Ty::add(const std::shared_ptr<const Base_type> type) {
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

int Ty::get_next_id() {
	return primes[id_generator++];
}
Ty Ty::get_void() {
	return { };
}
Ty Ty::get_any() {
	return { std::make_shared<Any_type>() };
}
Ty Ty::get_bool() {
	return { std::make_shared<Bool_type>() };
}
Ty Ty::get_number() {
	return { std::make_shared<Number_type>() };
}
Ty Ty::get_integer() {
	return { std::make_shared<Integer_type>() };
}
Ty Ty::get_real() {
	return { std::make_shared<Real_type>() };
}
Ty Ty::get_int() {
	return { std::make_shared<Int_type>() };
}
Ty Ty::get_long() {
	return { std::make_shared<Long_type>() };
}
Ty Ty::get_double() {
	return { std::make_shared<Double_type>() };
}
Ty Ty::get_mpz() {
	return { std::make_shared<Mpz_type>() };
}
Ty Ty::get_array() {
	return { std::make_shared<Array_type>() };
}
Ty Ty::get_array(const Ty& type) {
	return { std::make_shared<Array_type>(std::make_shared<Ty>(type)) };
}
Ty Ty::get_string() {
	return { std::make_shared<String_type>() };
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