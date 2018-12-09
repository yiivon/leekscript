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
#include "LLVMCompiler.hpp"
#include "value/Function.hpp"
#include "../vm/VM.hpp"
#include "../vm/value/LSNull.hpp"
#include "../vm/value/LSArray.hpp"
#include "../vm/value/LSMap.hpp"
#include "../vm/value/LSClosure.hpp"
#include "../colors.h"
#include "../type/RawType.hpp"

#define log_insn(i) log_instructions && _log_insn((i))

namespace ls {

llvm::LLVMContext LLVMCompiler::context;
llvm::IRBuilder<> LLVMCompiler::builder(context);

void LLVMCompiler::init() {
	mappings.clear();
}
void LLVMCompiler::end() {}

LLVMCompiler::value LLVMCompiler::clone(LLVMCompiler::value v) const {
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
LLVMCompiler::value LLVMCompiler::new_null() const {
	return new_pointer(LSNull::get());
}
LLVMCompiler::value LLVMCompiler::new_bool(bool b) const {
	return {llvm::ConstantInt::get(context, llvm::APInt(1, b, false)), Type::BOOLEAN};
}

LLVMCompiler::value LLVMCompiler::new_integer(int x) const {
	return {llvm::ConstantInt::get(context, llvm::APInt(32, x, true)), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::new_real(double r) const {
	return {llvm::ConstantFP::get(context, llvm::APFloat(r)), Type::REAL};
}

LLVMCompiler::value LLVMCompiler::new_long(long l) const {
	return {llvm::ConstantInt::get(context, llvm::APInt(64, l, true)), Type::LONG};
}

LLVMCompiler::value LLVMCompiler::new_pointer(const void* p, Type type) const {
	auto longp = llvm::ConstantInt::get(context, llvm::APInt(64, (long) p, false));
	return {builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, longp, type.llvm_type()), type};
}
LLVMCompiler::value LLVMCompiler::new_function(const void* p) const {
	return new_pointer(p, Type::FUNCTION);
}
LLVMCompiler::value LLVMCompiler::new_class(const void* p) const {
	return new_pointer(p, Type::CLASS);
}

LLVMCompiler::value LLVMCompiler::new_object() const {
	return insn_call(Type::OBJECT_TMP, {}, +[]() {
		// FIXME coverage doesn't work for the one line version
		auto o = new LSObject();
		return o;
	});
}

LLVMCompiler::value LLVMCompiler::new_object_class(LLVMCompiler::value clazz) const {
	return insn_call(Type::ANY, {clazz}, +[](LSClass* clazz) {
		return new LSObject(clazz);
	});
}

LLVMCompiler::value LLVMCompiler::new_mpz(long value) const {
	mpz_t mpz;
	mpz_init_set_ui(mpz, value);
	return new_mpz_init(mpz);
}

LLVMCompiler::value LLVMCompiler::new_mpz_init(const mpz_t mpz) const {
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

LLVMCompiler::value LLVMCompiler::new_array(Type type, std::vector<LLVMCompiler::value> elements) const {
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

LLVMCompiler::value LLVMCompiler::create_entry(const std::string& name, Type type) const {
	return { CreateEntryBlockAlloca(name, type.llvm_type()), type.pointer() };
}

LLVMCompiler::value LLVMCompiler::to_int(LLVMCompiler::value v) const {
	// assert(v.t.llvm_type() == v.v->getType());
	// assert(v.t.isNumber());
	if (v.t.is_integer()) {
		return v;
	}
	if (v.t.not_temporary() == Type::MPZ) {
		return to_int(insn_call(Type::LONG, {v}, +[](__mpz_struct a) {
			return mpz_get_si(&a);
		}));
	}
	if (v.t.not_temporary() == Type::BOOLEAN) {
		return {builder.CreateIntCast(v.v, Type::INTEGER.llvm_type(), false), Type::INTEGER};
	}
	if (v.t.is_real()) {
		return {builder.CreateFPToSI(v.v, Type::INTEGER.llvm_type()), Type::INTEGER};
	}
	LLVMCompiler::value r {builder.CreateIntCast(v.v, Type::INTEGER.llvm_type(), true), Type::INTEGER};
	log_insn(4) << "to_int " << dump_val(v) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::to_real(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t.is_real()) {
		return x;
	}
	if (x.t.is_bool()) {
		return {builder.CreateUIToFP(x.v, Type::REAL.llvm_type()), Type::REAL};
	}
	return {builder.CreateSIToFP(x.v, Type::REAL.llvm_type()), Type::REAL};
}

LLVMCompiler::value LLVMCompiler::to_long(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(v.t.isNumber());
	if (v.t.not_temporary() == Type::LONG) {
		return v;
	}
	if (v.t.not_temporary() == Type::INTEGER) {
		return {builder.CreateIntCast(v.v, Type::LONG.llvm_type(), true), Type::LONG};
	}
	assert(false && "not converted...");
	return v;
}

LLVMCompiler::value LLVMCompiler::insn_convert(LLVMCompiler::value v, Type t) const {
	// assert(v.t.llvm_type() == v.v->getType());
	if (!v.v) { return v; }
	if (!v.t.isNumber() && !t.isNumber()) {
		return { builder.CreatePointerCast(v.v, t.llvm_type()), t };
	}
	if (v.t.isNumber() && !t.isNumber()) {
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
LLVMCompiler::value LLVMCompiler::insn_not(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	return {builder.CreateNot(to_int(v).v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_not_bool(LLVMCompiler::value v) const {
	LLVMCompiler::value r {builder.CreateNot(insn_to_bool(v).v), Type::BOOLEAN};
	log_insn(4) << "not_bool " << dump_val(v) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_neg(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t == Type::REAL) {
		return {builder.CreateFNeg(v.v), v.t};
	} else {
		return {builder.CreateNeg(v.v), v.t};
	}
}

LLVMCompiler::value LLVMCompiler::insn_and(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	return {builder.CreateAnd(a.v, b.v), Type::BOOLEAN};
}
LLVMCompiler::value LLVMCompiler::insn_or(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	return {builder.CreateOr(a.v, b.v), Type::BOOLEAN};
}

LLVMCompiler::value LLVMCompiler::insn_add(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	if (a.t.is_real() or b.t.is_real()) {
		return {builder.CreateFAdd(to_real(a).v, to_real(b).v, "add"), Type::REAL};
	} else if (a.t.is_long() or b.t.is_long()) {
		return {builder.CreateAdd(to_long(a).v, to_long(b).v, "add"), Type::LONG};
	} else {
		return {builder.CreateAdd(to_int(a).v, to_int(b).v, "add"), Type::INTEGER};
	}
}

LLVMCompiler::value LLVMCompiler::insn_sub(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	if (a.t.is_real() or b.t.is_real()) {
		return {builder.CreateFSub(to_real(a).v, to_real(b).v, "sub"), Type::REAL};
	} else {
		return {builder.CreateSub(to_int(a).v, to_int(b).v, "sub"), Type::INTEGER};
	}
}

LLVMCompiler::value LLVMCompiler::insn_eq(LLVMCompiler::value a, LLVMCompiler::value b) const {
	// assert(a.t.llvm_type() == a.v->getType());
	// assert(b.t.llvm_type() == b.v->getType());
	if (!a.t.isNumber() or !b.t.isNumber()) {
		return insn_call(Type::BOOLEAN, {insn_to_any(a), insn_to_any(b)}, +[](LSValue* x, LSValue* y) {
			bool r = *x == *y;
			LSValue::delete_temporary(x);
			LSValue::delete_temporary(y);
			return r;
		});
	}
	if (a.t.is_real() or b.t.is_real()) {
		return {builder.CreateFCmpOEQ(to_real(a).v, to_real(b).v), Type::BOOLEAN};
	} else if (a.t.is_long() or b.t.is_long()) {
		return {builder.CreateICmpEQ(to_long(a).v, to_long(b).v), Type::BOOLEAN};
	} else {
		return {builder.CreateICmpEQ(to_int(a).v, to_int(b).v), Type::BOOLEAN};
	}
}

LLVMCompiler::value LLVMCompiler::insn_ne(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	LLVMCompiler::value r {builder.CreateICmpNE(a.v, b.v), Type::BOOLEAN};
	log_insn(4) << "ne " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_lt(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	LLVMCompiler::value r;
	if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOLT(to_real(a).v, to_real(b).v), Type::BOOLEAN};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSLT(to_long(a).v, to_long(b).v), Type::BOOLEAN};
	} else {
		r = {builder.CreateICmpSLT(a.v, b.v), Type::BOOLEAN};
	}
	log_insn(4) << "lt " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_le(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	LLVMCompiler::value r;
	if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOLE(to_real(a).v, to_real(b).v), Type::BOOLEAN};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSLE(to_long(a).v, to_long(b).v), Type::BOOLEAN};
	} else {
		r = {builder.CreateICmpSLE(a.v, b.v), Type::BOOLEAN};
	}
	log_insn(4) << "le " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_gt(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	LLVMCompiler::value r;
	if (a.t.is_real() || b.t.is_real()) {
		r = {builder.CreateFCmpOGT(to_real(a).v, to_real(b).v), Type::BOOLEAN};
	} else if (a.t.is_long() || b.t.is_long()) {
		r = {builder.CreateICmpSGT(to_long(a).v, to_long(b).v), Type::BOOLEAN};
	} else {
		r = {builder.CreateICmpSGT(a.v, b.v), Type::BOOLEAN};
	}
	log_insn(4) << "gt " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_ge(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	LLVMCompiler::value r;
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

LLVMCompiler::value LLVMCompiler::insn_mul(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	if (a.t.is_real() or b.t.is_real()) {
		return {builder.CreateFMul(to_real(a).v, to_real(b).v, "multmp"), Type::REAL};
	} else if (a.t.is_long() or b.t.is_long()) {
		return {builder.CreateMul(to_long(a).v, to_long(b).v), Type::LONG};
	}
	return {builder.CreateMul(to_int(a).v, to_int(b).v, "multmp"), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_div(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.isNumber() && b.t.isNumber());
	return {builder.CreateFDiv(to_real(a).v, to_real(b).v), Type::REAL};
}

LLVMCompiler::value LLVMCompiler::insn_int_div(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	if (a.t.is_long() or b.t.is_long()) {
		return {builder.CreateSDiv(to_long(a).v, to_long(b).v), Type::LONG};
	}
	return {builder.CreateSDiv(a.v, b.v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_bit_and(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	return {builder.CreateAnd(a.v, b.v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_bit_or(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	return {builder.CreateOr(a.v, b.v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_bit_xor(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	return {builder.CreateXor(a.v, b.v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_shl(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	return {builder.CreateShl(a.v, b.v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_ashr(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	return {builder.CreateAShr(a.v, b.v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_lshr(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	return {builder.CreateLShr(a.v, b.v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_mod(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	if (a.t.is_long() || b.t.is_long()) {
		return {builder.CreateSRem(a.v, b.v), Type::LONG};
	} else {
		return {builder.CreateSRem(a.v, b.v), Type::INTEGER};
	}
}

LLVMCompiler::value LLVMCompiler::insn_cmpl(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	return {builder.CreateSub(to_int(insn_to_bool(a)).v, to_int(insn_to_bool(b)).v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_log(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
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

LLVMCompiler::value LLVMCompiler::insn_log10(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
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

LLVMCompiler::value LLVMCompiler::insn_ceil(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) return x;
	return insn_call(Type::INTEGER, {x}, +[](double x) {
		return (int) std::ceil(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_round(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) return x;
	return insn_call(Type::INTEGER, {x}, +[](double x) {
		return (int) std::round(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_floor(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) return x;
	return insn_call(Type::INTEGER, {x}, +[](double x) {
		return (int) std::floor(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_cos(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::cos(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::cos(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_sin(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::sin(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::sin(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_tan(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::tan(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::tan(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_acos(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::acos(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::acos(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_asin(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::asin(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::asin(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_atan(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::atan(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::atan(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_sqrt(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::REAL, {x}, +[](int x) {
			return std::sqrt(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::sqrt(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_pow(LLVMCompiler::value a, LLVMCompiler::value b) const {
	assert(a.t.llvm_type() == a.v->getType());
	assert(b.t.llvm_type() == b.v->getType());
	assert(a.t.isNumber() && b.t.isNumber());
	LLVMCompiler::value r;
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

LLVMCompiler::value LLVMCompiler::insn_min(LLVMCompiler::value x, LLVMCompiler::value y) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(y.t.llvm_type() == y.v->getType());
	assert(x.t.isNumber() && y.t.isNumber());
	if (x.t == Type::INTEGER and y.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x, y}, +[](int x, int y) {
			return std::min(x, y);
		});
	}
	return insn_call(Type::REAL, {to_real(x), to_real(y)}, +[](double x, double y) {
		return std::min(x, y);
	});
}

LLVMCompiler::value LLVMCompiler::insn_max(LLVMCompiler::value x, LLVMCompiler::value y) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(y.t.llvm_type() == y.v->getType());
	assert(x.t.isNumber() && y.t.isNumber());
	if (x.t == Type::INTEGER and y.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x, y}, +[](int x, int y) {
			return std::max(x, y);
		});
	}
	return insn_call(Type::REAL, {to_real(x), to_real(y)}, +[](double x, double y) {
		return std::max(x, y);
	});
}

LLVMCompiler::value LLVMCompiler::insn_exp(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::exp(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::exp(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_atan2(LLVMCompiler::value x, LLVMCompiler::value y) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(y.t.llvm_type() == y.v->getType());
	assert(x.t.isNumber() && y.t.isNumber());
	if (x.t == Type::INTEGER && y.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x, y}, +[](int x, int y) {
			return std::atan2(x, y);
		});
	}
	return insn_call(Type::REAL, {to_real(x), to_real(y)}, +[](double x, double y) {
		return std::atan2(x, y);
	});
}

LLVMCompiler::value LLVMCompiler::insn_abs(LLVMCompiler::value x) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(x.t.isNumber());
	if (x.t == Type::INTEGER) {
		return insn_call(Type::INTEGER, {x}, +[](int x) {
			return std::abs(x);
		});
	}
	return insn_call(Type::REAL, {x}, +[](double x) {
		return std::fabs(x);
	});
}

LLVMCompiler::value LLVMCompiler::insn_to_any(LLVMCompiler::value v) const {
	if (!v.t.isNumber()) {
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

LLVMCompiler::value LLVMCompiler::insn_to_bool(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.is_bool()) {
		return v;
	}
	if (v.t.is_integer()) {
		LLVMCompiler::value r {builder.CreateICmpNE(v.v, llvm::Constant::getNullValue(v.v->getType())), Type::BOOLEAN};
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

LLVMCompiler::value LLVMCompiler::insn_load(LLVMCompiler::value v) const {
	// std::cout << "insn_load " << v.t << " " << v.v->getType() << std::endl;
	assert(v.t.llvm_type() == v.v->getType());
	assert(v.t.is_pointer());
	LLVMCompiler::value r { builder.CreateLoad(v.v), v.t.pointed() };
	log_insn(4) << "load " << dump_val(v) << " " << dump_val(r) << std::endl;
	assert(r.t == v.t.pointed());
	assert(r.t.llvm_type() == r.v->getType());
	return r;
}
LLVMCompiler::value LLVMCompiler::insn_load_member(LLVMCompiler::value v, int pos) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(v.t.is_pointer());
	assert(v.t.pointed().is_struct());
	auto s = builder.CreateStructGEP(v.t.pointed().llvm_type(), v.v, pos);
	return { builder.CreateLoad(s), v.t.pointed().member(pos) };
}

void LLVMCompiler::insn_store(LLVMCompiler::value x, LLVMCompiler::value y) const {
	// std::cout << "insn_store " << x.t << " " << x.v->getType() << " " << y.t << std::endl;
	assert(x.v->getType()->isPointerTy());
	// assert(x.t.llvm_type() == x.v->getType()->getPointerElementType());
	// assert(y.t.llvm_type() == y.v->getType());
	builder.CreateStore(y.v, x.v);
	log_insn(4) << "store " << dump_val(x) << " " << dump_val(y) << std::endl;
}
void LLVMCompiler::insn_store_member(LLVMCompiler::value x, int pos, LLVMCompiler::value y) const {
	assert(x.t.llvm_type() == x.v->getType());
	assert(y.t.llvm_type() == y.v->getType());
	assert(x.t.is_pointer());
	assert(x.t.pointed().is_struct());
	assert(x.t.pointed().member(pos) == y.t);
	auto s = builder.CreateStructGEP(x.t.pointed().llvm_type(), x.v, pos);
	builder.CreateStore(y.v, s);
}

LLVMCompiler::value LLVMCompiler::insn_typeof(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.is_null()) return new_integer(LSValue::NULLL);
	if (v.t.is_bool()) return new_integer(LSValue::BOOLEAN);
	if (v.t.isNumber()) return new_integer(LSValue::NUMBER);
	if (v.t.is_string()) return new_integer(LSValue::STRING);
	if (v.t.is_array()) return new_integer(LSValue::ARRAY);
	if (v.t.is_map()) return new_integer(LSValue::MAP);
	if (v.t.is_set()) return new_integer(LSValue::SET);
	if (v.t.is_interval()) return new_integer(LSValue::INTERVAL);
	if (v.t.is_closure()) return new_integer(LSValue::CLOSURE);
	if (v.t.is_function()) return new_integer(LSValue::FUNCTION);
	if (v.t.is_object()) return new_integer(LSValue::OBJECT);
	if (v.t.is_class()) return new_integer(LSValue::CLASS);
	return insn_call(Type::INTEGER, {v}, +[](LSValue* v) {
		return v->type;
	});
}

LLVMCompiler::value LLVMCompiler::insn_class_of(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	auto clazz = v.t.clazz();
	if (clazz.size()) {
		return new_pointer(vm->system_vars[clazz]);
	} else {
		return insn_call(Type::CLASS, {v}, +[](LSValue* v) {
			return v->getClass();
		});
	}
}

void LLVMCompiler::insn_delete(LLVMCompiler::value v) const {
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

void LLVMCompiler::insn_delete_temporary(LLVMCompiler::value v) const {
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

LLVMCompiler::value LLVMCompiler::insn_array_size(LLVMCompiler::value v) const {
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

LLVMCompiler::value LLVMCompiler::insn_get_capture(int index, Type type) const {
	// std::cout << "get_capture " << fun << " " << F->arg_size() << " " << type << " " << F->arg_begin()->getType() << " " << index << std::endl;

	LLVMCompiler::value arg0 = {F->arg_begin(), Type::INTEGER};

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

void LLVMCompiler::insn_push_array(LLVMCompiler::value array, LLVMCompiler::value value) const {
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

LLVMCompiler::value LLVMCompiler::insn_array_at(LLVMCompiler::value array, LLVMCompiler::value index) const {
	assert(array.t.llvm_type() == array.v->getType());
	assert(index.t.llvm_type() == index.v->getType());
	assert(array.t.is_array());
	assert(index.t.isNumber());
	auto array_type = array.v->getType()->getPointerElementType();
	auto raw_data = builder.CreateStructGEP(array_type, array.v, 5);
	auto data_base = builder.CreateLoad(raw_data);
	auto data_type = array.t.element().llvm_type()->getPointerTo();
	auto data = builder.CreatePointerCast(data_base, data_type);
	value r = { builder.CreateGEP(data, index.v), array.t.element().pointer() };
	assert(r.t.llvm_type() == r.v->getType());
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_array_end(LLVMCompiler::value array) const {
	assert(array.t.llvm_type() == array.v->getType());
	auto array_type = array.v->getType()->getPointerElementType();
	auto raw_data = builder.CreateStructGEP(array_type, array.v, 6);
	return {builder.CreateLoad(raw_data), array.t.iterator()};
}

LLVMCompiler::value LLVMCompiler::insn_move_inc(LLVMCompiler::value value) const {
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

LLVMCompiler::value LLVMCompiler::insn_clone_mpz(LLVMCompiler::value mpz) const {
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

void LLVMCompiler::insn_delete_mpz(LLVMCompiler::value mpz) const {
	assert(mpz.t.llvm_type() == mpz.v->getType());
	// std::cout << "delete mpz " << mpz.v->getType() << std::endl;
	// insn_call({}, {mpz}, &mpz_clear, "mpz_clear");
	// Increment mpz values counter
	// jit_value_t jit_counter_ptr = jit_value_create_long_constant(F, LS_POINTER, (long) &vm->mpz_deleted);
	// jit_value_t jit_counter = jit_insn_load_relative(F, jit_counter_ptr, 0, jit_type_long);
	// jit_insn_store_relative(F, jit_counter_ptr, 0, jit_insn_add(F, jit_counter, LS_CREATE_INTEGER(F, 1)));
}

LLVMCompiler::value LLVMCompiler::insn_inc_refs(LLVMCompiler::value v) const {
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

LLVMCompiler::value LLVMCompiler::insn_dec_refs(LLVMCompiler::value v, LLVMCompiler::value previous) const {
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

LLVMCompiler::value LLVMCompiler::insn_move(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	if (v.t.must_manage_memory() and !v.t.temporary and !v.t.reference) {
		return insn_call(v.t, {v}, (void*) +[](LSValue* v) {
			return v->move();
		});
	}
	return v;
}
LLVMCompiler::value LLVMCompiler::insn_refs(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(v.t.must_manage_memory());
	auto llvm_type = v.v->getType()->getPointerElementType();
	auto r = builder.CreateStructGEP(llvm_type, v.v, 3);
	return {builder.CreateLoad(r), Type::INTEGER};
}
LLVMCompiler::value LLVMCompiler::insn_native(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(v.t.must_manage_memory());
	auto llvm_type = v.v->getType()->getPointerElementType();
	auto r = builder.CreateStructGEP(llvm_type, v.v, 4);
	return {builder.CreateLoad(r), Type::BOOLEAN};
}

LLVMCompiler::value LLVMCompiler::insn_get_argument(const std::string& name) const {
	return arguments.top().at(name);
}

LLVMCompiler::value LLVMCompiler::iterator_begin(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	log_insn_code("iterator.begin()");
	if (v.t.is_array()) {
		auto it = create_entry("it", v.t.iterator());
		insn_store(it, insn_load_member(v, 5));
		return it;
	}
	else if (v.t.is_interval()) {
		// jit_type_t types[2] = {jit_type_void_ptr, jit_type_int};
		// auto interval_iterator = jit_type_create_struct(types, 2, 1);
		// Compiler::value it = {jit_value_create(F, interval_iterator), Type::INTERVAL_ITERATOR};
		// jit_type_free(interval_iterator);
		// auto addr = insn_address_of(it);
		// jit_insn_store_relative(F, addr.v, 0, v.v);
		// jit_insn_store_relative(F, addr.v, 8, insn_load(v, 20, Type::INTEGER).v);
		// return it;
	}
	else if (v.t.is_string()) {
		// jit_type_t types[5] = {jit_type_void_ptr, jit_type_int, jit_type_int, jit_type_int, jit_type_int};
		// auto string_iterator = jit_type_create_struct(types, 5, 1);
		// Compiler::value it = {jit_value_create(F, string_iterator), Type::STRING_ITERATOR};
		// jit_type_free(string_iterator);
		// auto addr = insn_address_of(it);
		// insn_call({}, {v, addr}, (void*) +[](LSString* str, LSString::iterator* it) {
		// 	auto i = LSString::iterator_begin(str);
		// 	it->buffer = i.buffer;
		// 	it->index = 0;
		// 	it->pos = 0;
		// 	it->next_pos = 0;
		// 	it->character = 0;
		// });
		// return it;
	}
	else if (v.t.is_map()) {
		// return insn_load(v, 48, v.t);
	}
	else if (v.t.is_set()) {
		auto it = create_entry("it", v.t.iterator());
		insn_call({}, {v}, +[](LSSet<int>* set) {
			std::cout << "set data() " << set->begin()._M_node << std::endl;
		});
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

LLVMCompiler::value LLVMCompiler::iterator_end(LLVMCompiler::value v, LLVMCompiler::value it) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(it.t.llvm_type() == it.v->getType());
	log_insn_code("iterator.end()");
	if (v.t.is_array()) {
		return {builder.CreateICmpEQ(insn_load(it).v, insn_array_end(v).v), Type::BOOLEAN};
	}
	else if (it.t == Type::INTERVAL_ITERATOR) {
		// auto addr = insn_address_of(it);
		// auto interval = insn_load(addr, 0, Type::POINTER);
		// auto end = insn_load(interval, 24, Type::INTEGER);
		// auto pos = insn_load(addr, 8, Type::INTEGER);
		// return insn_gt(pos, end);
	}
	else if (it.t == Type::STRING_ITERATOR) {
		// auto addr = insn_address_of(it);
		// return insn_call(Type::BOOLEAN, {addr}, &LSString::iterator_end);
	}
	else if (v.t.is_map()) {
		auto end = insn_add(v, new_integer(32)); // end_ptr = &map + 24
		return insn_eq(it, end);
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

LLVMCompiler::value LLVMCompiler::iterator_get(Type collectionType, LLVMCompiler::value it, LLVMCompiler::value previous) const {
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
		insn_inc_refs({f.v, collectionType.element()});
		return f;
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		// auto addr = insn_address_of(it);
		// auto e = insn_load(addr, 8, Type::INTEGER);
		// return e;
	}
	if (it.t == Type::STRING_ITERATOR) {
		// auto addr = insn_address_of(it);
		// auto int_char = insn_call(Type::INTEGER, {addr}, &LSString::iterator_get);
		// return insn_call(Type::STRING, {int_char, previous}, (void*) +[](unsigned int c, LSString* previous) {
		// 	if (previous != nullptr) {
		// 		LSValue::delete_ref(previous);
		// 	}
		// 	char dest[5];
		// 	// u8_toutf8(dest, 5, &c, 1);
		// 	auto s = new LSString(dest);
		// 	s->refs = 1;
		// 	return s;
		// });
	}
	if (it.t.is_map()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, +[](LSValue* previous) {
				if (previous != nullptr)
					LSValue::delete_ref(previous);
			});
		}
		// auto e = insn_load(it, 32 + 8, it.t.element());
		// insn_inc_refs(e);
		// return e;
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

LLVMCompiler::value LLVMCompiler::iterator_key(LLVMCompiler::value v, LLVMCompiler::value it, LLVMCompiler::value previous) const {
	assert(v.t.llvm_type() == v.v->getType());
	assert(it.t.llvm_type() == it.v->getType());
	// assert(previous.t.llvm_type() == previous.v->getType());
	log_insn_code("iterator.key()");
	if (v.t.is_array()) {
		auto array_begin = insn_array_at(v, new_integer(0));
		return { builder.CreatePtrDiff(insn_load(it).v, array_begin.v), Type::ANY };
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		// auto addr = insn_address_of(it);
		// auto interval = insn_load(addr, 0);
		// auto start = insn_load(interval, 20);
		// auto e = insn_load(addr, 8, Type::INTEGER);
		// return insn_sub(e, start);
	}
	if (it.t == Type::STRING_ITERATOR) {
		// auto addr = insn_address_of(it);
		// return insn_call(Type::INTEGER, {addr}, &LSString::iterator_key);
	}
	if (it.t.is_map()) {
		if (previous.t.must_manage_memory()) {
			insn_call({}, {previous}, +[](LSValue* previous) {
				if (previous != nullptr)
					LSValue::delete_ref(previous);
			});
		}
		// auto key = insn_load(it, 32, it.t.getKeyType());
		// insn_inc_refs(key);
		// return key;
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

void LLVMCompiler::iterator_increment(Type collectionType, LLVMCompiler::value it) const {
	assert(it.t.llvm_type() == it.v->getType());
	log_insn_code("iterator.increment()");
	if (collectionType.is_array()) {
		auto it2 = insn_load(it);
		auto next_element = builder.CreateGEP(it2.v, new_integer(1).v);
		insn_store(it, {next_element, Type::ANY});
		return;
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		// auto addr = insn_address_of(it);
		// auto pos = insn_load(addr, 8, Type::INTEGER);
		// insn_store_relative(addr, 8, insn_add(pos, new_integer(1)));
		return;
	}
	if (it.t == Type::STRING_ITERATOR) {
		// auto addr = insn_address_of(it);
		// insn_call({}, {addr}, &LSString::iterator_next);
		// return;
	}
	if (it.t.is_map()) {
		insn_store(it, insn_call(Type::ANY, {it}, (void*) +[](LSMap<int, int>::iterator it) {
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
LLVMCompiler::label LLVMCompiler::insn_init_label(std::string name, llvm::Function* fun) const {
	return {llvm::BasicBlock::Create(LLVMCompiler::context, name, fun)};
}
void LLVMCompiler::insn_if(LLVMCompiler::value condition, std::function<void()> then) const {
	assert(condition.t.llvm_type() == condition.v->getType());
	auto label_then = insn_init_label("then");
	auto label_end = insn_init_label("ifcont");
	insn_if_new(insn_to_bool(condition), &label_then, &label_end);
	insn_label(&label_then);
	then();
	insn_branch(&label_end);
	insn_label(&label_end);
}
void LLVMCompiler::insn_if_new(LLVMCompiler::value cond, label* then, label* elze) const {
	assert(cond.t.llvm_type() == cond.v->getType());
	builder.CreateCondBr(cond.v, then->block, elze->block);
}

void LLVMCompiler::insn_if_not(LLVMCompiler::value condition, std::function<void()> then) const {
	assert(condition.t.llvm_type() == condition.v->getType());
	auto label_then = insn_init_label("then");
	auto label_end = insn_init_label("ifcont");
	insn_if_new(insn_to_bool(condition), &label_end, &label_then);
	insn_label(&label_then);
	then();
	insn_branch(&label_end);
	insn_label(&label_end);
}

void LLVMCompiler::insn_throw(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	insn_call({}, {v}, +[](int type) {
		throw vm::ExceptionObj((vm::Exception) type);
	});
}

void LLVMCompiler::insn_throw_object(vm::Exception type) const {
	insn_call({}, {new_integer(type)}, +[](int type) {
		throw vm::ExceptionObj((vm::Exception) type);
	});
}

void LLVMCompiler::insn_label(label* l) const {
	auto function = LLVMCompiler::builder.GetInsertBlock()->getParent();
	function->getBasicBlockList().push_back(l->block);
	builder.SetInsertPoint(l->block);
}

void LLVMCompiler::insn_branch(label* l) const {
	builder.CreateBr(l->block);
}

void LLVMCompiler::insn_branch_if_pc_not_in_range(label* a, label* b, label* n) const { assert(false); }

void LLVMCompiler::insn_return(LLVMCompiler::value v) const {
	assert(v.t.llvm_type() == v.v->getType());
	builder.CreateRet(v.v);
}
void LLVMCompiler::insn_return_void() const {
	builder.CreateRetVoid();
}

// Call functions
void fake_ex_destru(void*) {}
LLVMCompiler::value LLVMCompiler::insn_call(Type return_type, std::vector<LLVMCompiler::value> args, void* func, bool exception) const {
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
		((LLVMCompiler*) this)->mappings.insert({function_name, {(llvm::JITTargetAddress) func, lambdaFN}});
	}
	const auto lambda = mappings.at(function_name).function;

	if (exception) {
		auto continueBlock = llvm::BasicBlock::Create(context, "cont", F);
		auto catchBlock = llvm::BasicBlock::Create(context, "catch", F);

		auto savedIP = builder.saveAndClearIP();
		auto lpad = llvm::BasicBlock::Create(context, "lpad", F);
		builder.SetInsertPoint(lpad);
		auto catchAllSelector = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context));
		auto LPadInst = builder.CreateLandingPad(llvm::StructType::get(llvm::Type::getInt8PtrTy(context), llvm::Type::getInt32Ty(context)), 1);
		auto LPadExn = builder.CreateExtractValue(LPadInst, 0);
		auto ExceptionSlot = CreateEntryBlockAlloca("exn.slot", llvm::Type::getInt64Ty(context));
		builder.CreateStore(LPadExn, ExceptionSlot);
		LPadInst->addClause(catchAllSelector);
		builder.CreateBr(catchBlock);
		builder.restoreIP(savedIP);

		value r = {builder.CreateInvoke(lambda, continueBlock, lpad, llvm_args, function_name), return_type};

		builder.SetInsertPoint(catchBlock);
		delete_function_variables();
		insn_call({}, {{builder.CreateLoad(ExceptionSlot), Type::LONG}}, +[](void* ex) {
			__cxa_throw((ex + 32), (void*) &typeid(vm::ExceptionObj), &fake_ex_destru);
		});
		builder.CreateBr(continueBlock);

		builder.SetInsertPoint(continueBlock);
		assert(r.t.llvm_type() == r.v->getType());
		return r;
	} else {
		value r = { builder.CreateCall(mappings.at(function_name).function, llvm_args, function_name), return_type };
		assert(r.t.llvm_type() == r.v->getType());
		return r;
	}
}

LLVMCompiler::value LLVMCompiler::insn_call_indirect(Type return_type, LLVMCompiler::value fun, std::vector<LLVMCompiler::value> args) const {
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		assert(args[i].t.llvm_type() == args[i].v->getType());
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t.llvm_type());
	}
	auto fun_type = llvm::FunctionType::get(return_type.llvm_type(), llvm_types, false);
	auto fun_conv = builder.CreatePointerCast(fun.v, fun_type->getPointerTo());
	return {builder.CreateCall(fun_type, fun_conv, llvm_args, "call"), return_type};
}

void LLVMCompiler::function_add_capture(LLVMCompiler::value fun, LLVMCompiler::value capture) {
	assert(fun.t.llvm_type() == fun.v->getType());
	assert(capture.t.llvm_type() == capture.v->getType());
	// std::cout << "add capture " << capture.t << std::endl;
	insn_call({}, {fun, capture}, +[](LSClosure* fun, LSValue* cap) {
		// std::cout << "add capture value " << cap << std::endl;
		fun->add_capture(cap); 
	});
}

void LLVMCompiler::log(const std::string&& str) const { assert(false); }

// Blocks
void LLVMCompiler::enter_block() {
	variables.push_back(std::map<std::string, value> {});
	if (!loops_blocks.empty()) {
		loops_blocks.back()++;
	}
	functions_blocks.back()++;
}
void LLVMCompiler::leave_block() {
	delete_variables_block(1);
	variables.pop_back();
	if (!loops_blocks.empty()) {
		loops_blocks.back()--;
	}
	functions_blocks.back()--;
}
void LLVMCompiler::delete_variables_block(int deepness) {
	for (int i = variables.size() - 1; i >= (int) variables.size() - deepness; --i) {
		for (auto it = variables[i].begin(); it != variables[i].end(); ++it) {
			// std::cout << "delete variable block " << it->first << " " << it->second.t << " " << it->second.v->getType() << std::endl;
			insn_delete(insn_load(it->second));
		}
	}
}

void LLVMCompiler::enter_function(llvm::Function* F, bool is_closure, Function* fun) {
	variables.push_back(std::map<std::string, value> {});
	function_variables.push_back(std::vector<value> {});
	functions.push(F);
	functions2.push(fun);
	functions_blocks.push_back(0);
	catchers.push_back({});
	function_is_closure.push(is_closure);
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

void LLVMCompiler::leave_function() {
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
	builder.SetInsertPoint(this->fun->block);
	log_insn(0) << "}" << std::endl;
}

int LLVMCompiler::get_current_function_blocks() const {
	return functions_blocks.back();
}
void LLVMCompiler::delete_function_variables() const {
	for (const auto& v : function_variables.back()) {
		insn_delete(insn_load(v));
	}
}
bool LLVMCompiler::is_current_function_closure() const {
	return function_is_closure.size() ? function_is_closure.top() : false;
}

// Variables
LLVMCompiler::value LLVMCompiler::add_var(const std::string& name, LLVMCompiler::value value) {
	assert((value.v != nullptr) && "value must not be null");
	assert(value.t.llvm_type() == value.v->getType());
	auto var = create_entry(name, value.t);
	insn_store(var, value);
	variables.back()[name] = var;
	return var;
}

LLVMCompiler::value LLVMCompiler::create_and_add_var(const std::string& name, Type type) {
	// std::cout << "Compiler::create_and_add_var(" << name << ", " << type << ")" << std::endl;
	auto var = create_entry(name, type);
	variables.back()[name] = var;
	return var;
}

void LLVMCompiler::add_function_var(LLVMCompiler::value value) {
	assert(value.t.llvm_type() == value.v->getType());
	function_variables.back().push_back(value);
}

LLVMCompiler::value LLVMCompiler::get_var(const std::string& name) {
	for (int i = variables.size() - 1; i >= 0; --i) {
		auto it = variables[i].find(name);
		if (it != variables[i].end()) {
			return it->second;
		}
	}
	assert(false && "var not found !");
	return *((LLVMCompiler::value*) nullptr); // Should not reach this line
}

void LLVMCompiler::set_var_type(std::string& name, const Type& type) {
	for (int i = variables.size() - 1; i >= 0; --i) {
		auto it = variables[i].find(name);
		if (it != variables[i].end()) {
			variables[i][name].t = type;
			return;
		}
	}
}

void LLVMCompiler::update_var(std::string& name, LLVMCompiler::value v) {
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
void LLVMCompiler::enter_loop(LLVMCompiler::label* end_label, LLVMCompiler::label* cond_label) {
	loops_end_labels.push_back(end_label);
	loops_cond_labels.push_back(cond_label);
	loops_blocks.push_back(0);
}
void LLVMCompiler::leave_loop() {
	loops_end_labels.pop_back();
	loops_cond_labels.pop_back();
	loops_blocks.pop_back();
}
LLVMCompiler::label* LLVMCompiler::get_current_loop_end_label(int deepness) const {
	return loops_end_labels[loops_end_labels.size() - deepness];
}
LLVMCompiler::label* LLVMCompiler::get_current_loop_cond_label(int deepness) const {
	return loops_end_labels[loops_end_labels.size() - deepness];
}
int LLVMCompiler::get_current_loop_blocks(int deepness) const {
	int sum = 0;
	for (size_t i = loops_blocks.size() - deepness; i < loops_blocks.size(); ++i) {
		sum += loops_blocks[i];
	}
	return sum;
}

/** Operations **/
void LLVMCompiler::inc_ops(int add) const {
	// TODO
}
void LLVMCompiler::inc_ops_jit(LLVMCompiler::value add) const {
	assert(add.t.llvm_type() == add.v->getType());
	assert(add.t.isNumber());
	// TODO
}
void LLVMCompiler::get_operations() const { assert(false); }

/** Exceptions **/
void LLVMCompiler::mark_offset(int line) {
	// TODO
}
void LLVMCompiler::add_catcher(label start, label end, label handler) { assert(false); }

void LLVMCompiler::insn_check_args(std::vector<LLVMCompiler::value> args, std::vector<LSValueType> types) const {
	// TODO too much cheks sometimes
	for (size_t i = 0; i < args.size(); ++i) {
		auto arg = args[i];
		// assert(arg.t.llvm_type() == arg.v->getType());
		auto type = types[i];
		if (!arg.t.isNumber() and type != arg.t.id() and type != 0) {
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
std::ostringstream& LLVMCompiler::_log_insn(int indent) const { assert(false); }

std::string LLVMCompiler::dump_val(LLVMCompiler::value v) const { assert(false); }

void LLVMCompiler::register_label(label* v) const { assert(false); }

void LLVMCompiler::log_insn_code(std::string instruction) const {
	log_insn(0) << C_BLUE << instruction << END_COLOR << std::endl;
}
void LLVMCompiler::add_literal(void* ptr, std::string value) const {
	((LLVMCompiler*) this)->literals.insert({ptr, value});
}

void LLVMCompiler::print_mpz(__mpz_struct value) {
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
