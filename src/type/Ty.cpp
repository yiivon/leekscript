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
#include "Array_type.hpp"
#include "String_type.hpp"

namespace ls {

Ty::Ty() : ptr(std::make_shared<LType>()) {}
Ty::Ty(Base_type* type) : ptr(std::make_shared<LType>(type)) {}

Ty::~Ty() {}

bool Ty::operator == (const Ty& ty) const {
	return *ptr == *ty.ptr;
}

Ty Ty::get_void() {
	return { };
}
Ty Ty::get_any() {
	return { new Any_type() };
}
Ty Ty::get_bool() {
	return { new Bool_type() };
}
Ty Ty::get_number() {
	return { new Number_type() };
}
Ty Ty::get_integer() {
	return { new Integer_type() };
}
Ty Ty::get_real() {
	return { new Real_type() };
}
Ty Ty::get_int() {
	return { new Int_type() };
}
Ty Ty::get_long() {
	return { new Long_type() };
}
Ty Ty::get_double() {
	return { new Double_type() };
}
Ty Ty::get_mpz() {
	return { new Mpz_type() };
}
Ty Ty::get_array() {
	return { new Array_type() };
}
Ty Ty::get_string() {
	return { new String_type() };
}

std::ostream& operator << (std::ostream& os, const Ty& ty) {
	os << ty.ptr;
	return os;
}

}