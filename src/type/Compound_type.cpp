#include "Compound_type.hpp"
#include "../colors.h"

namespace ls {

Compound_type::Compound_type(std::set<const Type*> list, const Type* folded) {
	for (const auto& t : list) types.push_back(t);
	this->folded = folded;
}
bool Compound_type::operator == (const Type* type) const {
	return false;
}
bool Compound_type::callable() const {
	return all([](const Type* t) {
		return t->callable();
	});
}
bool Compound_type::container() const {
	return all([](const Type* t) {
		return t->container();
	});
}
bool Compound_type::all(std::function<bool(const Type*)> fun) const {
	return std::all_of(types.begin(), types.end(), fun);
}
bool Compound_type::some(std::function<bool(const Type*)> fun) const {
	return std::any_of(types.begin(), types.end(), fun);
}
int Compound_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	return folded->distance(type->folded);
}
void Compound_type::implement(const Type* type) const {
	for (const auto& t : types) {
		t->implement(type);
	}
}
llvm::Type* Compound_type::llvm(const Compiler& c) const {
	return folded->llvm(c);
}

std::string Compound_type::class_name() const {
	return folded->class_name();
}
const std::string Compound_type::getName() const {
	std::string r;
	for (size_t i = 0; i < types.size(); ++i) {
		if (i > 0) { r += " | "; }
		r += types[i]->getName();
	}
	return r;
}
std::ostream& Compound_type::print(std::ostream& os) const {
	for (size_t i = 0; i < types.size(); ++i) {
		if (i > 0) { os << " | "; }
		types[i]->print(os);
	}
	return os;
}
Type* Compound_type::clone() const {
	std::set<const Type*> t;
	for (const auto& tt : types) t.insert(tt);
	return new Compound_type { t, folded };
}

}