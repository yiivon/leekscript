#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <bitset>
#include "Compiler.hpp"
#include "value/Function.hpp"
#include "../vm/VM.hpp"
#include "../vm/value/LSNull.hpp"
#include "../vm/value/LSArray.hpp"
#include "../vm/value/LSMap.hpp"
#include "../vm/value/LSClosure.hpp"
#include "../colors.h"
#include "../type/RawType.hpp"
#include "leekscript/lib/utf8.h"
#include "semantic/SemanticAnalyser.hpp"

#define log_insn(i) log_instructions && _log_insn((i))

namespace ls {

llvm::LLVMContext Compiler::context;
llvm::IRBuilder<> Compiler::builder(context);

void Compiler::init() {
	mappings.clear();
}
void Compiler::end() {}

Compiler::value Compiler::clone(Compiler::value v) const {
	if (v.t.must_manage_memory()) {
		if (v.t.reference) {
			v = insn_load(v);
		}
		auto r = insn_call(v.t, {v}, +[](LSValue* value) {
			return value->clone();
		});
		// log_insn(4) << "clone " << dump_val(v) << " " << dump_val(r) << std::endl;
		return r;
	}
	return v;
}
Compiler::value Compiler::new_null() const {
	return new_pointer(LSNull::get(), Type::NULLL);
}
Compiler::value Compiler::new_bool(bool b) const {
	return {llvm::ConstantInt::get(context, llvm::APInt(1, b, false)), Type::BOOLEAN};
}

Compiler::value Compiler::new_integer(int x) const {
	return {llvm::ConstantInt::get(context, llvm::APInt(32, x, true)), Type::INTEGER};
}

Compiler::value Compiler::new_real(double r) const {
	return {llvm::ConstantFP::get(context, llvm::APFloat(r)), Type::REAL};
}

Compiler::value Compiler::new_long(long l) const {
	return {llvm::ConstantInt::get(context, llvm::APInt(64, l, true)), Type::LONG};
}

Compiler::value Compiler::new_pointer(const void* p, Type type) const {
	assert(type.is_pointer());
	auto longp = llvm::ConstantInt::get(context, llvm::APInt(64, (long) p, false));
	return { builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, longp, type.llvm_type()), type };
}
Compiler::value Compiler::new_function(const void* p, Type type) const {
	return new_pointer(p, type);
}
Compiler::value Compiler::new_class(const void* p) const {
	return new_pointer(p, Type::CLASS);
}

Compiler::value Compiler::new_object() const {
	return insn_call(Type::OBJECT_TMP, {}, +[]() {
		// FIXME coverage doesn't work for the one line version
		auto o = new LSObject();
		return o;
	});
}

Compiler::value Compiler::new_object_class(Compiler::value clazz) const {
	return insn_call(Type::ANY, {clazz}, +[](LSClass* clazz) {
		return new LSObject(clazz);
	});
}

Compiler::value Compiler::new_mpz(long value) const {
	mpz_t mpz;
	mpz_init_set_ui(mpz, value);
	return new_mpz_init(mpz);
}

Compiler::value Compiler::new_mpz_init(const mpz_t mpz) const {
	// std::cout << "mpz alloc = " << mpz->_mp_alloc << std::endl;
	// std::cout << "mpz size = " << mpz->_mp_size << std::endl;
	// std::cout << "mpz d = " << mpz->_mp_d << std::endl;
	unsigned long p1 = (((unsigned long) mpz->_mp_d >> 32) << 32) + (((unsigned long) mpz->_mp_d << 32) >> 32);
	unsigned long p2 = (((unsigned long) mpz->_mp_size) << 32) + (unsigned long) mpz->_mp_alloc;
	//std::cout << "p1 = " << std::bitset<64>(p1) << std::endl;
	//std::cout << "p2 = " << std::bitset<64>(p2) << std::endl;
	auto v = llvm::ConstantInt::get(context, llvm::APInt(128, {p2, p1}));
	Compiler::value vv {v, Type::MPZ};
	// insn_call({}, {vv}, +[](__mpz_struct mpz) {
	// 	std::cout << "mpz alloc = " << mpz._mp_alloc << std::endl;
	// 	std::cout << "mpz size = " << mpz._mp_size << std::endl;
	// 	std::cout << "mpz d = " << mpz._mp_d << std::endl;
	// });
	return vv;
	// return {builder.CreateIntCast(v, Type::LLVM_MPZ_TYPE, false), Type::MPZ};
}

Compiler::value Compiler::new_array(Type type, std::vector<Compiler::value> elements) const {
	auto element_type = type.element().fold();
	auto array = [&]() { if (element_type == Type::INTEGER) {
		return insn_call(type.add_temporary(), {new_integer(elements.size())}, +[](int capacity) {
			auto array = new LSArray<int>();
			array->reserve(capacity);
			return array;
		});
	} else if (element_type == Type::REAL) {
		return insn_call(type.add_temporary(), {new_integer(elements.size())}, +[](int capacity) {
			auto array = new LSArray<double>();
			array->reserve(capacity);
			return array;
		});
	} else {
		return insn_call(type.add_temporary(), {new_integer(elements.size())}, +[](int capacity) {
			auto array = new LSArray<LSValue*>();
			array->reserve(capacity);
			return array;
		});
	}}();
	for (const auto& element : elements) {
		auto v = insn_move(insn_convert(element, element_type));
		insn_push_array(array, v);
	}
	// size of the array + 1 operations
	inc_ops(elements.size() + 1);
	return array;
}

Compiler::value Compiler::create_entry(const std::string& name, Type type) const {
	return { CreateEntryBlockAlloca(name, type.llvm_type()), type.pointer() };
}

Compiler::value Compiler::to_int(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.is_polymorphic()) {
		return insn_invoke(Type::INTEGER, {v}, +[](const LSValue* x) {
			if (auto number = dynamic_cast<const LSNumber*>(x)) {
				auto r = (int) number->value;
				LSValue::delete_temporary(x);
				return r;
			} else if (auto boolean = dynamic_cast<const LSBoolean*>(x)) {
				auto r = boolean->value ? 1 : 0;
				LSValue::delete_temporary(x);
				return r;
			}
			LSValue::delete_temporary(x);
			throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
		});
	}
	if (v.t.is_integer()) {
		return v;
	}
	if (v.t.not_temporary() == Type::MPZ) {
		return to_int(insn_call(Type::LONG, {v}, +[](__mpz_struct a) {
			return mpz_get_si(&a);
		}));
	}
	if (v.t.is_bool()) {
		return {builder.CreateIntCast(v.v, Type::INTEGER.llvm_type(), false), Type::INTEGER};
	}
	if (v.t.is_real()) {
		return {builder.CreateFPToSI(v.v, Type::INTEGER.llvm_type()), Type::INTEGER};
	}
	Compiler::value r {builder.CreateIntCast(v.v, Type::INTEGER.llvm_type(), true), Type::INTEGER};
	log_insn(4) << "to_int " << dump_val(v) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::to_real(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	if (x.t.is_polymorphic()) {
		return insn_invoke(Type::REAL, {x}, +[](const LSValue* x) {
			if (auto number = dynamic_cast<const LSNumber*>(x)) {
				auto r = number->value;
				LSValue::delete_temporary(x);
				return r;
			} else if (auto boolean = dynamic_cast<const LSBoolean*>(x)) {
				auto r = boolean->value ? 1.0 : 0.0;
				LSValue::delete_temporary(x);
				return r;
			}
			LSValue::delete_temporary(x);
			throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
		});
	}
	if (x.t.is_real()) {
		return x;
	}
	if (x.t.is_bool()) {
		return {builder.CreateUIToFP(x.v, Type::REAL.llvm_type()), Type::REAL};
	}
	return {builder.CreateSIToFP(x.v, Type::REAL.llvm_type()), Type::REAL};
}

Compiler::value Compiler::to_long(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.not_temporary() == Type::LONG) {
		return v;
	}
	if (v.t.is_bool()) {
		return {builder.CreateIntCast(v.v, Type::LONG.llvm_type(), false), Type::LONG};
	}
	if (v.t.not_temporary() == Type::INTEGER) {
		return {builder.CreateIntCast(v.v, Type::LONG.llvm_type(), true), Type::LONG};
	}
	assert(false && "not converted...");
	return v;
}

Compiler::value Compiler::insn_convert(Compiler::value v, Type t) const {
	// assert(v.t.llvm_type() == v.v->getType());
	if (!v.v) { return v; }
	if (v.t.is_polymorphic() and t.is_polymorphic()) {
		return { builder.CreatePointerCast(v.v, t.llvm_type()), t };
	}
	if (v.t.is_primitive() && t.is_polymorphic()) {
		return insn_to_any(v);
	}
	if (v.t.not_temporary() == t.not_temporary()) return v;
	if (t == Type::REAL) {
		return to_real(v);
	} else if (t == Type::INTEGER) {
		return to_int(v);
	} else if (t == Type::LONG) {
		return to_long(v);
	}
	return v;
}

// Operators wrapping
Compiler::value Compiler::insn_not(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	return {builder.CreateNot(to_int(v).v), Type::INTEGER};
}

Compiler::value Compiler::insn_not_bool(Compiler::value v) const {
	Compiler::value r {builder.CreateNot(insn_to_bool(v).v), Type::BOOLEAN};
	log_insn(4) << "not_bool " << dump_val(v) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_neg(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t == Type::REAL) {
		return {builder.CreateFNeg(v.v), v.t};
	} else {
		return {builder.CreateNeg(v.v), v.t};
	}
}

Compiler::value Compiler::insn_and(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	return {builder.CreateAnd(a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_or(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	return {builder.CreateOr(a.v, b.v), Type::BOOLEAN};
}

Compiler::value Compiler::insn_add(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	if (a.t.is_real() or b.t.is_real()) {
		return {builder.CreateFAdd(to_real(a).v, to_real(b).v), Type::REAL};
	} else if (a.t.is_long() or b.t.is_long()) {
		return {builder.CreateAdd(to_long(a).v, to_long(b).v), Type::LONG};
	} else {
		return {builder.CreateAdd(to_int(a).v, to_int(b).v), Type::INTEGER};
	}
}

Compiler::value Compiler::insn_sub(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	if (a.t.is_real() or b.t.is_real()) {
		return {builder.CreateFSub(to_real(a).v, to_real(b).v, "sub"), Type::REAL};
	} else {
		return {builder.CreateSub(to_int(a).v, to_int(b).v, "sub"), Type::INTEGER};
	}
}

Compiler::value Compiler::insn_eq(Compiler::value a, Compiler::value b) const {
	// assert(a.t.llvm_type() == a.v->getType());
	// assert(b.t.llvm_type() == b.v->getType());
	if (a.t.is_polymorphic() or b.t.is_polymorphic()) {
		return insn_call(Type::BOOLEAN, {insn_to_any(a), insn_to_any(b)}, +[](LSValue* x, LSValue* y) {
			bool r = *x == *y;
			LSValue::delete_temporary(x);
			LSValue::delete_temporary(y);
			return r;
		});
	}
	if (a.t.is_mpz() and b.t.is_integer()) {
		// TODO cleaning
		return insn_call(Type::BOOLEAN, {a, b}, +[](__mpz_struct x, int i) {
			return _mpz_cmp_si(&x, i) == 0;
		});
	} else if (a.t.is_mpz() and b.t.is_mpz()) {
		// TODO cleaning
		return insn_call(Type::BOOLEAN, {a, b}, +[](__mpz_struct x, __mpz_struct y) {
			return mpz_cmp(&x, &y) == 0;
		});
	} else if (a.t.is_real() or b.t.is_real()) {
		return {builder.CreateFCmpOEQ(to_real(a).v, to_real(b).v), Type::BOOLEAN};
	} else if (a.t.is_long() or b.t.is_long()) {
		return {builder.CreateICmpEQ(to_long(a).v, to_long(b).v), Type::BOOLEAN};
	} else {
		return {builder.CreateICmpEQ(to_int(a).v, to_int(b).v), Type::BOOLEAN};
	}
}

Compiler::value Compiler::insn_pointer_eq(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_pointer());
	assert(b.t.is_pointer());
	auto p1 = builder.CreatePointerCast(a.v, llvm::Type::getInt8PtrTy(Compiler::context));
	auto p2 = builder.CreatePointerCast(b.v, llvm::Type::getInt8PtrTy(Compiler::context));
	return { builder.CreateICmpEQ(p1, p2), Type::BOOLEAN };
}

Compiler::value Compiler::insn_ne(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	Compiler::value r {builder.CreateICmpNE(a.v, b.v), Type::BOOLEAN};
	log_insn(4) << "ne " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_lt(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	Compiler::value r;
	if (a.t.is_mpz() and b.t.is_integer()) {
		// TODO cleaning
		return insn_call(Type::BOOLEAN, {a, b}, +[](__mpz_struct x, int i) {
			return _mpz_cmp_si(&x, i) < 0;
		});
	} else if (a.t.is_mpz() and b.t.is_mpz()) {
		auto res = insn_call(Type::INTEGER, {a, b}, +[](__mpz_struct a, __mpz_struct b) {
			return mpz_cmp(&a, &b);
		});
		if (a.t.temporary) insn_delete_mpz(a);
		if (b.t.temporary) insn_delete_mpz(b);
		return insn_lt(res, new_integer(0));
	} else if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOLT(to_real(a).v, to_real(b).v), Type::BOOLEAN};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSLT(to_long(a).v, to_long(b).v), Type::BOOLEAN};
	} else {
		r = {builder.CreateICmpSLT(a.v, b.v), Type::BOOLEAN};
	}
	log_insn(4) << "lt " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_le(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	Compiler::value r;
	if (a.t.is_polymorphic() or b.t.is_polymorphic()) {
		r = {builder.CreateFCmpOLE(to_real(a).v, to_real(b).v), Type::BOOLEAN};
	} else if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOLE(to_real(a).v, to_real(b).v), Type::BOOLEAN};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSLE(to_long(a).v, to_long(b).v), Type::BOOLEAN};
	} else {
		r = {builder.CreateICmpSLE(a.v, b.v), Type::BOOLEAN};
	}
	log_insn(4) << "le " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_gt(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	Compiler::value r;
	if (a.t.is_mpz() and b.t.is_integer()) {
		auto res = insn_call(Type::INTEGER, {a, b}, +[](__mpz_struct a, int b) {
			return _mpz_cmp_si(&a, b);
		});
		return insn_lt(res, new_integer(0));
	} else if (a.t.is_integer() and b.t.is_mpz()) {
		auto res = insn_call(Type::INTEGER, {a, b}, +[](int a, __mpz_struct b) {
			return _mpz_cmp_si(&b, a);
		});
		return insn_lt(res, new_integer(0));
	} else if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOGT(to_real(a).v, to_real(b).v), Type::BOOLEAN};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSGT(to_long(a).v, to_long(b).v), Type::BOOLEAN};
	} else {
		r = {builder.CreateICmpSGT(a.v, b.v), Type::BOOLEAN};
	}
	log_insn(4) << "gt " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_ge(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	Compiler::value r;
	if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOGE(to_real(a).v, to_real(b).v), Type::BOOLEAN};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSGE(to_long(a).v, to_long(b).v), Type::BOOLEAN};
	} else {
		r = {builder.CreateICmpSGE(a.v, b.v), Type::BOOLEAN};
	}
	log_insn(4) << "ge " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_mul(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	// std::cout << "insn_mul " << a.t << " " << b.t << std::endl;
	if (a.t.is_real() or b.t.is_real()) {
		return {builder.CreateFMul(to_real(a).v, to_real(b).v), Type::REAL};
	} else if (a.t.is_long() or b.t.is_long()) {
		return {builder.CreateMul(to_long(a).v, to_long(b).v), Type::LONG};
	}
	return {builder.CreateMul(to_int(a).v, to_int(b).v), Type::INTEGER};
}

Compiler::value Compiler::insn_div(Compiler::value a, Compiler::value b) const {
	auto bv = to_real(b);
	insn_if(insn_eq(bv, new_integer(0)), [&]() {
		insn_delete_temporary(a);
		insn_delete_temporary(b);
		insn_throw_object(vm::Exception::DIVISION_BY_ZERO);
	});
	return { builder.CreateFDiv(to_real(a).v, bv.v), Type::REAL };
}

Compiler::value Compiler::insn_int_div(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	if (a.t.is_long() or b.t.is_long()) {
		return {builder.CreateSDiv(to_long(a).v, to_long(b).v), Type::LONG};
	}
	return {builder.CreateSDiv(a.v, b.v), Type::INTEGER};
}

Compiler::value Compiler::insn_bit_and(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	return {builder.CreateAnd(a.v, b.v), Type::INTEGER};
}

Compiler::value Compiler::insn_bit_or(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	return {builder.CreateOr(a.v, b.v), Type::INTEGER};
}

Compiler::value Compiler::insn_bit_xor(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	return {builder.CreateXor(a.v, b.v), Type::INTEGER};
}

Compiler::value Compiler::insn_shl(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	return {builder.CreateShl(a.v, b.v), Type::INTEGER};
}

Compiler::value Compiler::insn_ashr(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	return {builder.CreateAShr(a.v, b.v), Type::INTEGER};
}

Compiler::value Compiler::insn_lshr(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	return {builder.CreateLShr(a.v, b.v), Type::INTEGER};
}

Compiler::value Compiler::insn_mod(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	if (a.t.is_long() and b.t.is_long()) {
		return { builder.CreateSRem(a.v, b.v), Type::LONG };
	} else if (a.t.is_integer() and b.t.is_integer()) {
		return { builder.CreateSRem(a.v, b.v), Type::INTEGER };
	} else {
		return { builder.CreateFRem(to_real(a).v, to_real(b).v), Type::REAL };
	}
}

Compiler::value Compiler::insn_cmpl(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	return {builder.CreateSub(to_int(insn_to_bool(a)).v, to_int(insn_to_bool(b)).v), Type::INTEGER};
}

Compiler::value Compiler::insn_log(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::REAL, {x}, +[](int x) {
			return std::log(x);
		});
	}
	if (x.t == Type::LONG) {
		return insn_call(Type::REAL, {x}, +[](long x) {
			return std::log(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::log(x);
	});
}

Compiler::value Compiler::insn_log10(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::REAL, {x}, +[](int x) {
			return std::log10(x);
		});
	}
	if (x.t == Type::LONG) {
		return insn_call(Type::REAL, {x}, +[](long x) {
			return std::log10(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::log10(x);
	});
}

Compiler::value Compiler::insn_ceil(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) return x;
	return insn_call(Type::INTEGER, {x}, +[](double x) {
		return (int) std::ceil(x);
	});
}

Compiler::value Compiler::insn_round(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) return x;
	return insn_call(Type::INTEGER, {x}, +[](double x) {
		return (int) std::round(x);
	});
}

Compiler::value Compiler::insn_floor(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) return x;
	return insn_call(Type::INTEGER, {x}, +[](double x) {
		return (int) std::floor(x);
	});
}

Compiler::value Compiler::insn_cos(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::cos(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::cos(x);
	});
}

Compiler::value Compiler::insn_sin(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::sin(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::sin(x);
	});
}

Compiler::value Compiler::insn_tan(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::tan(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::tan(x);
	});
}

Compiler::value Compiler::insn_acos(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::acos(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::acos(x);
	});
}

Compiler::value Compiler::insn_asin(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::asin(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::asin(x);
	});
}

Compiler::value Compiler::insn_atan(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::atan(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::atan(x);
	});
}

Compiler::value Compiler::insn_sqrt(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::REAL, {x}, +[](int x) {
			return std::sqrt(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::sqrt(x);
	});
}

Compiler::value Compiler::insn_pow(Compiler::value a, Compiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.is_primitive() && b.t.is_primitive());
	Compiler::value r;
	if (a.t.is_real() or b.t.is_real()) {
		r = insn_call(Type::REAL, {to_real(a), to_real(b)}, +[](double a, double b) {
			return std::pow(a, b);
		});
	} else if (a.t.is_long()) {
		r = insn_call(Type::LONG, {a, to_int(b)}, +[](long a, int b) {
			return (long) std::pow(a, b);
		});
	} else if (a.t.is_integer()) {
		r = insn_call(Type::INTEGER, {a, b}, +[](int a, int b) {
			return (int) std::pow(a, b);
		});
	}
	log_insn(4) << "pow " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

Compiler::value Compiler::insn_min(Compiler::value x, Compiler::value y) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(y.t.llvm_type() == y.v->getType());
	assert(x.t.is_primitive() && y.t.is_primitive());
	if (x.t == Type::INTEGER and y.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x, y}, +[](int x, int y) {
			return std::min(x, y);
		});
	}
	return insn_call(Type::REAL, {to_real(x), to_real(y)}, +[](double x, double y) {
		return std::min(x, y);
	});
}

Compiler::value Compiler::insn_max(Compiler::value x, Compiler::value y) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(y.t.llvm_type() == y.v->getType());
	assert(x.t.is_primitive() && y.t.is_primitive());
	if (x.t == Type::INTEGER and y.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x, y}, +[](int x, int y) {
			return std::max(x, y);
		});
	}
	return insn_call(Type::REAL, {to_real(x), to_real(y)}, +[](double x, double y) {
		return std::max(x, y);
	});
}

Compiler::value Compiler::insn_exp(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.is_primitive());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::exp(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::exp(x);
	});
}

Compiler::value Compiler::insn_atan2(Compiler::value x, Compiler::value y) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(y.t.llvm_type() == y.v->getType());
	assert(x.t.is_primitive() && y.t.is_primitive());
	if (x.t == Type::INTEGER && y.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x, y}, +[](int x, int y) {
			return std::atan2(x, y);
		});
	}
	return insn_call(Type::REAL, {to_real(x), to_real(y)}, +[](double x, double y) {
		return std::atan2(x, y);
	});
}

Compiler::value Compiler::insn_abs(Compiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	// std::cout << "abs " << x.t << std::endl;
	if (x.t.is_polymorphic()) {
		return insn_call(Type::REAL, {to_real(x)}, +[](double x) {
			return std::fabs(x);
		});
	}
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::abs(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::fabs(x);
	});
}

Compiler::value Compiler::insn_to_any(Compiler::value v) const {
	if (v.t.is_polymorphic()) {
		return v; // already any
	}
	if (v.t.is_long()) {
		return insn_call(Type::ANY, {v}, +[](long n) {
			return LSNumber::get(n);
		});
	} else if (v.t.is_real()) {
		return insn_call(Type::ANY, {v}, +[](double n) {
			return LSNumber::get(n);
		});
	} else if (v.t.is_bool()) {
		return insn_call(Type::ANY, {v}, +[](bool n) {
			return LSBoolean::get(n);
		});
	} else {
		return insn_call(Type::ANY, {v}, +[](int n) {
			return LSNumber::get(n);
		});
	}
}

Compiler::value Compiler::insn_to_bool(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.is_bool()) {
		return v;
	}
	if (v.t.is_integer()) {
		Compiler::value r {builder.CreateICmpNE(v.v, llvm::Constant::getNullValue(v.v->getType())), Type::BOOLEAN};
		log_insn(4) << "to_bool " << dump_val(v) << " " << dump_val(r) << std::endl;
		return r;
	}
	if (v.t.is_string()) {
		return insn_call(Type::BOOLEAN, {v}, (void*) &LSString::to_bool);
	}
	if (v.t.is_array()) {
		// Always take LSArray<int>, but the array is not necessarily of this type
		return insn_call(Type::BOOLEAN, {v}, (void*) &LSArray<int>::to_bool);
	}
	if (v.t.is_function()) {
		return new_bool(true);
	}
	if (v.t.is_mpz()) {
		// TODO
		return v;
	}
	return insn_call(Type::BOOLEAN, {v}, +[](LSValue* v) {
		return v->to_bool();
	});
}

Compiler::value Compiler::insn_load(Compiler::value v) const {
	// std::cout << "insn_load " << v.t << " " << v.v->getType() << std::endl;
	assert(v.t.llvm_type() == v.v->getType());
	assert(v.t.is_pointer());
	Compiler::value r { builder.CreateLoad(v.v), v.t.pointed() };
	log_insn(4) << "load " << dump_val(v) << " " << dump_val(r) << std::endl;
	assert(r.t == v.t.pointed());
	assert(r.t.llvm_type() == r.v->getType());
	return r;
}
Compiler::value Compiler::insn_load_member(Compiler::value v, int pos) const {
	auto type = v.t.fold();
	assert(type.llvm_type() == v.v->getType());
	assert(type.fold().is_pointer());
	assert(type.fold().pointed().is_struct());
	auto s = builder.CreateStructGEP(type.pointed().llvm_type(), v.v, pos);
	return { builder.CreateLoad(s), type.pointed().member(pos) };
}

void Compiler::insn_store(Compiler::value x, Compiler::value y) const {
	// std::cout << "insn_store " << x.t << " " << x.v->getType() << " " << y.t << std::endl;
	assert(x.v->getType()->isPointerTy());
	// assert(x.t.llvm_type() == x.v->getType()->getPointerElementType());
	// assert(y.t.llvm_type() == y.v->getType());
	builder.CreateStore(y.v, x.v);
	log_insn(4) << "store " << dump_val(x) << " " << dump_val(y) << std::endl;
}
void Compiler::insn_store_member(Compiler::value x, int pos, Compiler::value y) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(y.t.llvm_type() == y.v->getType());
	assert(x.t.is_pointer());
	assert(x.t.pointed().is_struct());
	assert(x.t.pointed().member(pos) == y.t);
	auto s = builder.CreateStructGEP(x.t.pointed().llvm_type(), x.v, pos);
	builder.CreateStore(y.v, s);
}

Compiler::value Compiler::insn_typeof(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.fold().is_any()) {
		return insn_call(Type::INTEGER, {v}, +[](LSValue* v) {
			return v->type;
		});
	}
	return new_integer(v.t.id());
}

Compiler::value Compiler::insn_class_of(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	auto clazz = v.t.clazz();
	if (clazz.size()) {
		return new_pointer(vm->internal_vars.at(clazz)->lsvalue, Type::CLASS);
	} else {
		return insn_call(Type::CLASS, {v}, +[](LSValue* v) {
			return v->getClass();
		});
	}
}

void Compiler::insn_delete(Compiler::value v) const {
	// std::cout << "insn_delete " << v.t << " " << v.v->getType() << std::endl;
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.must_manage_memory()) {
		// insn_call({}, {v}, (void*) &LSValue::delete_ref);
		insn_if_not(insn_native(v), [&]() {
			auto refs = insn_refs(v);
			insn_if(insn_refs(v), [&]() {
				insn_if_not(insn_dec_refs(v, refs), [&]() {
					insn_call({}, {v}, (void*) &LSValue::free);
				});
			});
		});
	} else if (v.t.not_temporary() == Type::MPZ) {
		insn_delete_mpz(v);
	}
}

void Compiler::insn_delete_temporary(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.must_manage_memory()) {
		// insn_call({}, {v}, (void*) &LSValue::delete_temporary);
		insn_if_not(insn_refs(v), [&]() {
			insn_call({}, {v}, (void*) &LSValue::free);
		});
	} else if (v.t == Type::MPZ_TMP) {
		insn_delete_mpz(v);
	}
}

Compiler::value Compiler::insn_array_size(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.is_string()) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSString::int_size);
	} else if (v.t.is_array() and v.t.element() == Type::INTEGER) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<int>::int_size);
	} else if (v.t.is_array() and v.t.element() == Type::REAL) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<double>::int_size);
	} else {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<LSValue*>::int_size);
	}
	return {};
}

Compiler::value Compiler::insn_get_capture(int index, Type type) const {
	// std::cout << "get_capture " << fun << " " << F->arg_size() << " " << type << " " << F->arg_begin()->getType() << " " << index << std::endl;

	Compiler::value arg0 = {F->arg_begin(), Type::INTEGER};

	auto jit_index = new_integer(index);
	auto v = insn_call(Type::ANY, {arg0, jit_index}, +[](LSClosure* fun, int index) {
		// std::cout << "fun->get_capture(" << fun << ", " << index << ")" << std::endl;
		LSValue* v = fun->get_capture(index);
		// std::cout << "capture : " << ((LSNumber*) v)->value << std::endl;
		// v->refs++;
		return v;
	});
	if (type == Type::INTEGER) {
		v = insn_call(Type::INTEGER, {v}, +[](LSNumber* n) {
			return (int) n->value;
		});
	}
	return v;
}

void Compiler::insn_push_array(Compiler::value array, Compiler::value value) const {
	assert(array.t.llvm_type() == array.v->getType());
	assert(value.t.llvm_type() == value.v->getType());
	auto element_type = array.t.element().fold();
	if (element_type == Type::INTEGER) {
		insn_call({}, {array, value}, (void*) +[](LSArray<int>* array, int value) {
			array->push_back(value);
		});
	} else if (element_type == Type::REAL) {
		value.t = Type::REAL;
		insn_call({}, {array, value}, (void*) +[](LSArray<double>* array, double value) {
			array->push_back(value);
		});
	} else {
		insn_call({}, {array, value}, (void*) +[](LSArray<LSValue*>* array, LSValue* value) {
			array->push_inc(value);
		});
	}
}

Compiler::value Compiler::insn_array_at(Compiler::value array, Compiler::value index) const {
	assert(array.t.llvm_type() == array.v->getType());
	assert(index.t.llvm_type() == index.v->getType());
	assert(array.t.is_array());
	assert(index.t.is_number());
	auto array_type = array.v->getType()->getPointerElementType();
	auto raw_data = builder.CreateStructGEP(array_type, array.v, 5);
	auto data_base = builder.CreateLoad(raw_data);
	auto data_type = array.t.element().llvm_type()->getPointerTo();
	auto data = builder.CreatePointerCast(data_base, data_type);
	value r = { builder.CreateGEP(data, index.v), array.t.element().pointer() };
	assert(r.t.llvm_type() == r.v->getType());
	return r;
}

Compiler::value Compiler::insn_array_end(Compiler::value array) const {
	assert(array.t.llvm_type() == array.v->getType());
	return insn_load_member(array, 6);
}

Compiler::value Compiler::insn_move_inc(Compiler::value value) const {
	assert(value.t.llvm_type() == value.v->getType());
	if (value.t.must_manage_memory()) {
		if (value.t.reference) {
			insn_inc_refs(value);
			return value;
		} else {
			return insn_call(value.t, {value}, (void*) +[](LSValue* v) {
				return v->move_inc();
			});
		}
	}
	if (value.t.temporary) {
		return value;
	}
	if (value.t == Type::MPZ) {
		return insn_clone_mpz(value);
	} else {
		return value;
	}
}

Compiler::value Compiler::insn_clone_mpz(Compiler::value mpz) const {
	assert(mpz.t.llvm_type() == mpz.v->getType());
	return insn_call(Type::MPZ, {mpz}, +[](__mpz_struct x) {
		mpz_t new_mpz;
		mpz_init_set(new_mpz, &x);
		return *new_mpz;
	});
	// insn_call({}, {v}, +[](__mpz_struct mpz) {
	// 	std::cout << "mpz cloned alloc = " << mpz._mp_alloc << std::endl;
	// 	std::cout << "mpz cloned size = " << mpz._mp_size << std::endl;
	// 	std::cout << "mpz cloned d = " << mpz._mp_d << std::endl;
	// });
	// return v;
}

void Compiler::insn_delete_mpz(Compiler::value mpz) const {
	assert(mpz.t.llvm_type() == mpz.v->getType());
	// std::cout << "delete mpz " << mpz.v->getType() << std::endl;
	// insn_call({}, {mpz}, &mpz_clear, "mpz_clear");
	// Increment mpz values counter
	// jit_value_t jit_counter_ptr = jit_value_create_long_constant(F, LS_POINTER, (long) &vm->mpz_deleted);
	// jit_value_t jit_counter = jit_insn_load_relative(F, jit_counter_ptr, 0, jit_type_long);
	// jit_insn_store_relative(F, jit_counter_ptr, 0, jit_insn_add(F, jit_counter, LS_CREATE_INTEGER(F, 1)));
}

Compiler::value Compiler::insn_inc_refs(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.must_manage_memory()) {
		auto previous = insn_refs(v);
		auto new_refs = insn_add(previous, new_integer(1));
		auto llvm_type = v.v->getType()->getPointerElementType();
		auto r = builder.CreateStructGEP(llvm_type, v.v, 3);
		insn_store({r, Type::INTEGER}, new_refs);
		return new_refs;
	}
	return new_integer(0);
}

Compiler::value Compiler::insn_dec_refs(Compiler::value v, Compiler::value previous) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(previous.t.llvm_type() == previous.v->getType());
	if (v.t.must_manage_memory()) {
		if (previous.v == nullptr) {
			previous = insn_refs(v);
		}
		auto new_refs = insn_sub(previous, new_integer(1));
		auto llvm_type = v.v->getType()->getPointerElementType();
		auto r = builder.CreateStructGEP(llvm_type, v.v, 3);
		insn_store({r, Type::INTEGER}, new_refs);
		return new_refs;
	}
	return new_integer(0);
}

Compiler::value Compiler::insn_move(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.must_manage_memory() and !v.t.temporary and !v.t.reference) {
		return insn_call(v.t, {v}, (void*) +[](LSValue* v) {
			return v->move();
		});
	}
	return v;
}
Compiler::value Compiler::insn_refs(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(v.t.is_polymorphic());
	return insn_load_member(v, 3);
}
Compiler::value Compiler::insn_native(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(v.t.is_polymorphic());
	return insn_load_member(v, 4);
}

Compiler::value Compiler::insn_get_argument(const std::string& name) const {
	return arguments.top().at(name);
}

Compiler::value Compiler::iterator_begin(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	log_insn_code("iterator.begin()");
	if (v.t.is_array()) {
		auto it = create_entry("it", v.t.iterator());
		insn_store(it, insn_load_member(v, 5));
		return it;
	}
	else if (v.t.is_interval()) {
		auto it = create_entry("it", v.t.iterator());
		insn_store_member(it, 0, v);
		insn_store_member(it, 1, insn_load_member(v, 5));
		return it;
	}
	else if (v.t.is_string()) {
		auto it = create_entry("it", v.t.iterator());
		insn_call({}, {v, it}, (void*) +[](LSString* str, LSString::iterator* it) {
			auto i = LSString::iterator_begin(str);
			it->buffer = i.buffer;
			it->index = 0;
			it->pos = 0;
			it->next_pos = 0;
			it->character = 0;
		});
		return it;
	}
	else if (v.t.is_map()) {
		auto it = create_entry("it", v.t.iterator());
		insn_store(it, insn_load_member(v, 8));
		return it;
	}
	else if (v.t.is_set()) {
		auto it = create_entry("it", v.t.iterator());
		insn_store_member(it, 0, insn_load_member(v, 8));
		insn_store_member(it, 1, new_integer(0));
		return it;
	}
	else if (v.t.is_integer()) {
		auto it = create_entry("it", Type::INTEGER_ITERATOR);
		insn_store_member(it, 0, v);
		insn_store_member(it, 1, to_int(insn_pow(new_integer(10), to_int(insn_log10(v)))));
		insn_store_member(it, 2, new_integer(0));
		return it;
	}
	else if (v.t.is_long()) {
		auto it = create_entry("it", Type::LONG_ITERATOR);
		insn_store_member(it, 0, v);
		insn_store_member(it, 1, insn_pow(new_long(10), to_int(insn_log10(v))));
		insn_store_member(it, 2, new_integer(0));
		return it;
	}
	else if (v.t.is_mpz()) {
		// jit_type_t types[3] = {VM::mpz_type, VM::mpz_type, jit_type_int};
		// auto mpz_iterator = jit_type_create_struct(types, 3, 1);
		// Compiler::value it = {jit_value_create(F, mpz_iterator), Type::MPZ_ITERATOR};
		// jit_type_free(mpz_iterator);
		// auto addr = jit_insn_address_of(F, it.v);
		// jit_insn_store_relative(F, addr, 0, v.v);
		// jit_insn_store_relative(F, addr, 16, to_long(insn_pow(new_integer(10), to_int(insn_log10(v)))).v);
		// jit_insn_store_relative(F, addr, 32, new_long(0).v);
		// return it;
	}
	return {nullptr, {}};
}

Compiler::value Compiler::iterator_end(Compiler::value v, Compiler::value it) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(it.t.llvm_type() == it.v->getType());
	log_insn_code("iterator.end()");
	if (v.t.is_array()) {
		return insn_pointer_eq(insn_load(it), insn_array_end(v));
	}
	else if (v.t.is_interval()) {
		auto interval = insn_load_member(it, 0);
		auto end = insn_load_member(interval, 6);
		auto pos = insn_load_member(it, 1);
		return insn_gt(pos, end);
	}
	else if (v.t.is_string()) {
		return insn_call(Type::BOOLEAN, {it}, &LSString::iterator_end);
	}
	else if (v.t.is_map()) {
		auto node = insn_load(it);
		auto end = insn_call(node.t, {v}, +[](LSMap<int,int>* map) { return map->end()._M_node; });
		return { builder.CreateICmpEQ(node.v, end.v), Type::BOOLEAN };
	}
	else if (v.t.is_set()) {
		auto node = insn_load_member(it, 0);
		auto end = insn_call(node.t, {v}, +[](LSSet<int>* set) { return set->end()._M_node; });
		return {builder.CreateICmpEQ(node.v, end.v), Type::BOOLEAN};
	}
	else if (v.t == Type::INTEGER) {
		return insn_eq(insn_load_member(it, 1), new_integer(0));
	}
	else if (v.t == Type::LONG) {
		return insn_eq(insn_load_member(it, 1), new_long(0));
	}
	return {nullptr, {}};
}

Compiler::value Compiler::iterator_get(Type collectionType, Compiler::value it, Compiler::value previous) const {
	assert(it.t.llvm_type() == it.v->getType());
	assert(previous.t.llvm_type() == previous.v->getType());
	log_insn_code("iterator.get()");
	if (collectionType.is_array()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, +[](LSValue* previous) {
				if (previous != nullptr) {
					LSValue::delete_ref(previous);
				}
			});
		}
		auto e = insn_load(it);
		auto f = insn_load(e);
		insn_inc_refs(f);
		return f;
	}
	if (collectionType.is_interval()) {
		return insn_load_member(it, 1);
	}
	if (collectionType.is_string()) {
		auto int_char = insn_call(Type::INTEGER, {it}, &LSString::iterator_get);
		return insn_call(Type::STRING, {int_char, previous}, (void*) +[](unsigned int c, LSString* previous) {
			if (previous != nullptr) {
				LSValue::delete_ref(previous);
			}
			char dest[5];
			u8_toutf8(dest, 5, &c, 1);
			auto s = new LSString(dest);
			s->refs = 1;
			return s;
		});
	}
	if (collectionType.is_map()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, +[](LSValue* previous) {
				if (previous != nullptr)
					LSValue::delete_ref(previous);
			});
		}
		auto node = insn_load(it);
		auto e = insn_load_member(node, 5);
		insn_inc_refs(e);
		return e;
	}
	if (collectionType.is_set()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, +[](LSValue* previous) {
				if (previous != nullptr)
					LSValue::delete_ref(previous);
			});
		}
		auto node = insn_load_member(it, 0);
		auto e = insn_load_member(node, 4);
		insn_inc_refs(e);
		return e;
	}
	if (collectionType.is_integer()) {
		return insn_int_div(insn_load_member(it, 0), insn_load_member(it, 1));
	}
	else if (collectionType.is_long()) {
		return to_int(insn_int_div(insn_load_member(it, 0), insn_load_member(it, 1)));
	}
	return {nullptr, {}};
}

Compiler::value Compiler::iterator_key(Compiler::value v, Compiler::value it, Compiler::value previous) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(it.t.llvm_type() == it.v->getType());
	// assert(previous.t.llvm_type() == previous.v->getType());
	log_insn_code("iterator.key()");
	if (v.t.is_array()) {
		auto array_begin = insn_array_at(v, new_integer(0));
		return { builder.CreatePtrDiff(insn_load(it).v, array_begin.v), Type::ANY };
	}
	if (v.t.is_interval()) {
		auto interval = insn_load_member(it, 0);
		auto start = insn_load_member(interval, 5);
		auto e = insn_load_member(it, 1);
		return insn_sub(e, start);
	}
	if (v.t.is_string()) {
		return insn_call(Type::INTEGER, {it}, &LSString::iterator_key);
	}
	if (v.t.is_map()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, +[](LSValue* previous) {
				if (previous != nullptr) LSValue::delete_ref(previous);
			});
		}
		auto node = insn_load(it);
		auto e = insn_load_member(node, 4);
		insn_inc_refs(e);
		return e;
	}
	if (v.t.is_set()) {
		return insn_load_member(it, 1);
	}
	if (v.t.is_integer()) {
		return insn_load_member(it, 2);
	}
	else if (v.t.is_long()) {
		return insn_load_member(it, 2);
	}
	return {nullptr, {}};
}

void Compiler::iterator_increment(Type collectionType, Compiler::value it) const {
	assert(it.t.llvm_type() == it.v->getType());
	log_insn_code("iterator.increment()");
	if (collectionType.is_array()) {
		auto it2 = insn_load(it);
		auto next_element = builder.CreateGEP(it2.v, new_integer(1).v);
		insn_store(it, {next_element, Type::ANY});
		return;
	}
	if (collectionType.is_interval()) {
		auto pos = insn_load_member(it, 1);
		insn_store_member(it, 1, insn_add(pos, new_integer(1)));
		return;
	}
	if (collectionType.is_string()) {
		insn_call({}, {it}, &LSString::iterator_next);
		return;
	}
	if (collectionType.is_map()) {
		auto node = insn_load(it);
		insn_store(it, insn_call(node.t, {node}, (void*) +[](LSMap<int,int>::iterator it) {
			it++;
			return it;
		}));
		return;
	}
	if (collectionType.is_set()) {
		auto node = insn_load_member(it, 0);
		insn_store_member(it, 1, insn_add(insn_load_member(it, 1), new_integer(1)));
		insn_store_member(it, 0, insn_call(node.t, {node}, (void*) +[](LSSet<int>::iterator it) {
			it++;
			return it;
		}));
		return;
	}
	if (collectionType.is_integer()) {
		auto n = insn_load_member(it, 0);
		auto p = insn_load_member(it, 1);
		auto i = insn_load_member(it, 2);
		insn_store_member(it, 0, insn_mod(n, p));
		insn_store_member(it, 1, insn_int_div(p, new_integer(10)));
		insn_store_member(it, 2, insn_add(i, new_integer(1)));
	}
	else if (collectionType.is_long()) {
		auto n = insn_load_member(it, 0);
		auto p = insn_load_member(it, 1);
		auto i = insn_load_member(it, 2);
		insn_store_member(it, 0, insn_mod(n, p));
		insn_store_member(it, 1, insn_int_div(p, new_long(10)));
		insn_store_member(it, 2, insn_add(i, new_integer(1)));
	}
}

// Controls
Compiler::label Compiler::insn_init_label(std::string name) const {
	return {llvm::BasicBlock::Create(Compiler::context, name)};
}
void Compiler::insn_if(Compiler::value condition, std::function<void()> then) const {
	assert(condition.t.llvm_type() == condition.v->getType());
	auto label_then = insn_init_label("then");
	auto label_end = insn_init_label("ifcont");
	insn_if_new(insn_to_bool(condition), &label_then, &label_end);
	insn_label(&label_then);
	then();
	insn_branch(&label_end);
	insn_label(&label_end);
}
void Compiler::insn_if_new(Compiler::value cond, label* then, label* elze) const {
	assert(cond.t.llvm_type() == cond.v->getType());
	builder.CreateCondBr(cond.v, then->block, elze->block);
}

void Compiler::insn_if_not(Compiler::value condition, std::function<void()> then) const {
	assert(condition.t.llvm_type() == condition.v->getType());
	auto label_then = insn_init_label("then");
	auto label_end = insn_init_label("ifcont");
	insn_if_new(insn_to_bool(condition), &label_end, &label_then);
	insn_label(&label_then);
	then();
	insn_branch(&label_end);
	insn_label(&label_end);
}

void Compiler::insn_throw(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	delete_function_variables();
	insn_call({}, {v}, +[](int type) {
		throw vm::ExceptionObj((vm::Exception) type);
	});
}

void Compiler::insn_throw_object(vm::Exception type) const {
	insn_throw(new_integer(type));
}

void Compiler::insn_label(label* l) const {
	F->getBasicBlockList().push_back(l->block);
	builder.SetInsertPoint(l->block);
}

void Compiler::insn_branch(label* l) const {
	builder.CreateBr(l->block);
}

void Compiler::insn_return(Compiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	builder.CreateRet(v.v);
}
void Compiler::insn_return_void() const {
	builder.CreateRetVoid();
}

// Call functions
Compiler::value Compiler::insn_call(Type return_type, std::vector<Compiler::value> args, void* func) const {
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		// assert(args[i].t.llvm_type() == args[i].v->getType());
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t.llvm_type());
	}
	auto function_name = std::string("anonymous_func_") + std::to_string(mappings.size());
	auto i = mappings.find(function_name);
	if (i == mappings.end()) {
		auto fun_type = llvm::FunctionType::get(return_type.llvm_type(), llvm_types, false);
		auto lambdaFN = llvm::Function::Create(fun_type, llvm::Function::ExternalLinkage, function_name, fun->module.get());
		((Compiler*) this)->mappings.insert({function_name, {(llvm::JITTargetAddress) func, lambdaFN}});
	}
	value r = { builder.CreateCall(mappings.at(function_name).function, llvm_args, function_name), return_type };
	assert(r.t.llvm_type() == r.v->getType());
	return r;
}

Compiler::value Compiler::insn_invoke(Type return_type, std::vector<Compiler::value> args, void* func) const {
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		// assert(args[i].t.llvm_type() == args[i].v->getType());
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t.llvm_type());
	}
	auto function_name = std::string("anonymous_func_") + std::to_string(mappings.size());
	auto i = mappings.find(function_name);
	if (i == mappings.end()) {
		auto fun_type = llvm::FunctionType::get(return_type.llvm_type(), llvm_types, false);
		auto lambdaFN = llvm::Function::Create(fun_type, llvm::Function::ExternalLinkage, function_name, fun->module.get());
		((Compiler*) this)->mappings.insert({function_name, {(llvm::JITTargetAddress) func, lambdaFN}});
	}
	const auto lambda = mappings.at(function_name).function;
	auto continueBlock = llvm::BasicBlock::Create(context, "cont", F);
	value r = {builder.CreateInvoke(lambda, continueBlock, fun->get_landing_pad(*this), llvm_args, function_name), return_type};
	builder.SetInsertPoint(continueBlock);
	assert(r.t.llvm_type() == r.v->getType());
	return r;
}

Compiler::value Compiler::insn_call_indirect(Type return_type, Compiler::value fun, std::vector<Compiler::value> args) const {
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		assert(args[i].t.llvm_type() == args[i].v->getType());
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t.llvm_type());
	}
	auto fun_type = llvm::FunctionType::get(return_type.llvm_type(), llvm_types, false);
	auto fun_conv = builder.CreatePointerCast(fun.v, fun_type->getPointerTo());
	return {builder.CreateCall(fun_type, fun_conv, llvm_args), return_type};
}

void Compiler::function_add_capture(Compiler::value fun, Compiler::value capture) {
	assert(fun.t.llvm_type() == fun.v->getType());
	assert(capture.t.llvm_type() == capture.v->getType());
	// std::cout << "add capture " << capture.t << std::endl;
	insn_call({}, {fun, capture}, +[](LSClosure* fun, LSValue* cap) {
		// std::cout << "add capture value " << cap << std::endl;
		fun->add_capture(cap); 
	});
}

void Compiler::log(const std::string&& str) const { assert(false); }

// Blocks
void Compiler::enter_block() {
	variables.push_back(std::map<std::string, value> {});
	if (!loops_blocks.empty()) {
		loops_blocks.back()++;
	}
	functions_blocks.back()++;
}
void Compiler::leave_block(bool delete_vars) {
	if (delete_vars) {
		delete_variables_block(1);
	}
	variables.pop_back();
	if (!loops_blocks.empty()) {
		loops_blocks.back()--;
	}
	functions_blocks.back()--;
}
void Compiler::delete_variables_block(int deepness) {
	for (int i = variables.size() - 1; i >= (int) variables.size() - deepness; --i) {
		for (auto it = variables[i].begin(); it != variables[i].end(); ++it) {
			// std::cout << "delete variable block " << it->first << " " << it->second.t << " " << it->second.v->getType() << std::endl;
			insn_delete(insn_load(it->second));
		}
	}
}

void Compiler::enter_function(llvm::Function* F, bool is_closure, Function* fun) {
	variables.push_back(std::map<std::string, value> {});
	function_variables.push_back(std::vector<value> {});
	functions.push(F);
	functions2.push(fun);
	functions_blocks.push_back(0);
	catchers.push_back({});
	function_is_closure.push(is_closure);
	auto block = builder.GetInsertBlock();
	if (!block) block = fun->block;
	function_llvm_blocks.push(block);
	this->F = F;
	this->fun = fun;
	std::vector<std::string> args;
	log_insn(0) << "function " << fun->name << "(";
	for (unsigned i = 0; i < fun->arguments.size(); ++i) {
		log_insn(0) << fun->arguments.at(i)->content;
		if (i < fun->arguments.size() - 1) log_insn(0) << ", ";
		args.push_back(fun->arguments.at(i)->content);
	}
	arguments.push({});
	log_insn(0) << ") {" << std::endl;
}

void Compiler::leave_function() {
	variables.pop_back();
	function_variables.pop_back();
	functions.pop();
	functions2.pop();
	functions_blocks.pop_back();
	catchers.pop_back();
	function_is_closure.pop();
	arguments.pop();
	this->F = functions.top();
	this->fun = functions2.top();
	builder.SetInsertPoint(function_llvm_blocks.top());
	function_llvm_blocks.pop();
	log_insn(0) << "}" << std::endl;
}

int Compiler::get_current_function_blocks() const {
	return functions_blocks.back();
}
void Compiler::delete_function_variables() const {
	for (const auto& v : function_variables.back()) {
		insn_delete(insn_load(v));
	}
}
bool Compiler::is_current_function_closure() const {
	return function_is_closure.size() ? function_is_closure.top() : false;
}

// Variables
Compiler::value Compiler::add_var(const std::string& name, Compiler::value value) {
	assert((value.v != nullptr) && "value must not be null");
	assert(value.t.llvm_type() == value.v->getType());
	auto var = create_entry(name, value.t);
	insn_store(var, value);
	variables.back().at(name) = var;
	return var;
}

Compiler::value Compiler::create_and_add_var(const std::string& name, Type type) {
	// std::cout << "Compiler::create_and_add_var(" << name << ", " << type << ")" << std::endl;
	auto var = create_entry(name, type);
	variables.back()[name] = var;
	return var;
}

void Compiler::add_function_var(Compiler::value value) {
	assert(value.t.llvm_type() == value.v->getType());
	function_variables.back().push_back(value);
}

Compiler::value Compiler::get_var(const std::string& name) {
	for (int i = variables.size() - 1; i >= 0; --i) {
		auto it = variables[i].find(name);
		if (it != variables[i].end()) {
			return it->second;
		}
	}
	assert(false && "var not found !");
	return *((Compiler::value*) nullptr); // Should not reach this line
}

void Compiler::update_var(std::string& name, Compiler::value v) {
	assert(v.t.llvm_type() == v.v->getType());
	for (int i = variables.size() - 1; i >= 0; --i) {
		if (variables[i].find(name) != variables[i].end()) {
			auto& var = variables[i].at(name);
			if (var.t != v.t.pointer()) {
				var.t = v.t.pointer();
				var.v = CreateEntryBlockAlloca(name, v.t.llvm_type());
			}
			insn_store(var, v);
		}
	}
}

// Loops
void Compiler::enter_loop(Compiler::label* end_label, Compiler::label* cond_label) {
	loops_end_labels.push_back(end_label);
	loops_cond_labels.push_back(cond_label);
	loops_blocks.push_back(0);
}
void Compiler::leave_loop() {
	loops_end_labels.pop_back();
	loops_cond_labels.pop_back();
	loops_blocks.pop_back();
}
Compiler::label* Compiler::get_current_loop_end_label(int deepness) const {
	return loops_end_labels[loops_end_labels.size() - deepness];
}
Compiler::label* Compiler::get_current_loop_cond_label(int deepness) const {
	return loops_end_labels[loops_end_labels.size() - deepness];
}
int Compiler::get_current_loop_blocks(int deepness) const {
	int sum = 0;
	for (size_t i = loops_blocks.size() - deepness; i < loops_blocks.size(); ++i) {
		sum += loops_blocks[i];
	}
	return sum;
}

/** Operations **/
void Compiler::inc_ops(int amount) const {
	inc_ops_jit(new_integer(amount));
}
void Compiler::inc_ops_jit(Compiler::value amount) const {
	assert(amount.t.llvm_type() == amount.v->getType());
	assert(amount.t.is_number());

	// Operations enabled?
	if (not vm->enable_operations) return;

	// Variable counter pointer
	auto ops_ptr = new_pointer(&vm->operations, Type::INTEGER.pointer());

	// Increment counter
	auto jit_ops = insn_load(ops_ptr);

	// Compare to the limit
	auto limit = new_long(vm->operation_limit);
	auto compare = insn_gt(jit_ops, limit);
	// If greater than the limit, throw exception
	insn_if(compare, [&]() {
		insn_throw_object(vm::Exception::OPERATION_LIMIT_EXCEEDED);
	});
	insn_store(ops_ptr, insn_add(jit_ops, amount));
}

/** Exceptions **/
void Compiler::mark_offset(int line) {
	// TODO
}
void Compiler::add_catcher(label start, label end, label handler) { assert(false); }

void Compiler::insn_check_args(std::vector<Compiler::value> args, std::vector<LSValueType> types) const {
	// TODO too much cheks sometimes
	for (size_t i = 0; i < args.size(); ++i) {
		auto arg = args[i];
		assert(arg.t.llvm_type() == arg.v->getType());
		auto type = types[i];
		if (arg.t.is_polymorphic() and type != arg.t.id() and type != 0) {
			auto type = types[i];
			insn_if(insn_ne(insn_typeof(arg), new_integer(type)), [&]() {
				for (auto& a : args) {
					insn_delete_temporary(a);
				}
				insn_throw_object(vm::Exception::WRONG_ARGUMENT_TYPE);
			});
		}
	}
}

// Utils
std::ostringstream& Compiler::_log_insn(int indent) const { assert(false); }

std::string Compiler::dump_val(Compiler::value v) const { assert(false); }

void Compiler::register_label(label* v) const { assert(false); }

void Compiler::log_insn_code(std::string instruction) const {
	log_insn(0) << C_BLUE << instruction << END_COLOR << std::endl;
}
void Compiler::add_literal(void* ptr, std::string value) const {
	((Compiler*) this)->literals.insert({ptr, value});
}

void Compiler::print_mpz(__mpz_struct value) {
	char buff[10000];
	mpz_get_str(buff, 10, &value);
	std::cout << buff;
}

}

namespace std {
	std::ostream& operator << (std::ostream& os, const __mpz_struct v) {
		char buff[10000];
		mpz_get_str(buff, 10, &v);
		os << buff;
		return os;
	}
}
