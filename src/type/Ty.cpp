#include "Ty.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>

namespace ls {

int Ty::id_generator = 0;
int Ty::primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151 };
std::map<int, std::shared_ptr<Base_type>> Ty::type_map;

void Ty::init_types() {
	auto add = [](std::shared_ptr<Base_type> type) {
		type_map.emplace(type->id(), type);
	};
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
	// auto new_type = Ty::get_void();
	// for (const auto& t : _types) {
	// 	new_type.add(t);
	// }
	// for (const auto& t : type._types) {
	// 	new_type.add(t);
	// }
	// return new_type;
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
	
}

int Ty::get_next_id() {
	return primes[id_generator++];
}

}