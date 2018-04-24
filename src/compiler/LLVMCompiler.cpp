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
#include "LLVMCompiler.hpp"
#include "value/Function.hpp"
#include "../vm/VM.hpp"
#include "../vm/value/LSNull.hpp"
#include "../vm/value/LSArray.hpp"
#include "../vm/value/LSMap.hpp"
#include "../vm/value/LSClosure.hpp"

#define log_insn(i) log_instructions && _log_insn((i))

namespace ls {

llvm::LLVMContext LLVMCompiler::context;
llvm::IRBuilder<> LLVMCompiler::Builder(context);
std::unique_ptr<llvm::Module> LLVMCompiler::TheModule;
std::map<std::string, llvm::Value*> LLVMCompiler::NamedValues;
std::unique_ptr<llvm::legacy::FunctionPassManager> LLVMCompiler::TheFPM;

void LLVMCompiler::init() {

	// Open a new module.
	TheModule = llvm::make_unique<llvm::Module>("my_jit", context);
	TheModule->setDataLayout(getTargetMachine().createDataLayout());

	// Create a new pass manager attached to it.
	TheFPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	TheFPM->add(llvm::createInstructionCombiningPass());
	// Reassociate expressions.
	TheFPM->add(llvm::createReassociatePass());
	// Eliminate Common SubExpressions.
	TheFPM->add(llvm::createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	TheFPM->add(llvm::createCFGSimplificationPass());
	TheFPM->doInitialization();

	mappings.clear();

	Type::LLVM_LSVALUE_TYPE = llvm::StructType::create("lsvalue", llvm::Type::getInt32Ty(LLVMCompiler::context), llvm::Type::getInt32Ty(LLVMCompiler::context), llvm::Type::getInt32Ty(LLVMCompiler::context), llvm::Type::getInt32Ty(LLVMCompiler::context), llvm::Type::getInt1Ty(LLVMCompiler::context));
	Type::LLVM_LSVALUE_TYPE_PTR = Type::LLVM_LSVALUE_TYPE->getPointerTo();
	Type::LLVM_LSVALUE_TYPE_PTR_PTR = Type::LLVM_LSVALUE_TYPE_PTR->getPointerTo();

	Type::LLVM_MPZ_TYPE = llvm::StructType::create("mpz", llvm::Type::getInt32Ty(LLVMCompiler::context), llvm::Type::getInt32Ty(LLVMCompiler::context), llvm::Type::getInt32PtrTy(LLVMCompiler::context));
	Type::LLVM_MPZ_TYPE_PTR = Type::LLVM_MPZ_TYPE->getPointerTo();

	Type::LLVM_VECTOR_TYPE = llvm::StructType::create("mpz", llvm::Type::getInt32PtrTy(LLVMCompiler::context), llvm::Type::getInt32PtrTy(LLVMCompiler::context), llvm::Type::getInt32PtrTy(LLVMCompiler::context), Type::LLVM_LSVALUE_TYPE_PTR_PTR);
	Type::LLVM_VECTOR_TYPE_PTR = Type::LLVM_VECTOR_TYPE->getPointerTo();
}

void LLVMCompiler::end() {
}

LLVMCompiler::value LLVMCompiler::clone(LLVMCompiler::value v) const {
	if (v.t.must_manage_memory()) {
		if (v.t.reference) {
			v = insn_load(v);
		}
		auto r = insn_call(v.t, {v}, +[](LSValue* value) {
			return value->clone();
		}, "clone");
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

LLVMCompiler::value LLVMCompiler::new_pointer(const void* p) const {
	auto longp = llvm::ConstantInt::get(context, llvm::APInt(64, (long) p, false));
	return {Builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, longp, Type::LLVM_LSVALUE_TYPE_PTR), Type::POINTER};
}

LLVMCompiler::value LLVMCompiler::new_object() const {
	return insn_call(Type::OBJECT_TMP, {}, +[]() {
		// FIXME coverage doesn't work for the one line version
		auto o = new LSObject();
		return o;
	}, "new_object");
}

LLVMCompiler::value LLVMCompiler::new_object_class(LLVMCompiler::value clazz) const {
	return insn_call(Type::POINTER, {clazz}, +[](LSClass* clazz) {
		return new LSObject(clazz);
	});
}

LLVMCompiler::value LLVMCompiler::new_mpz(long value) const {
	auto mpz_struct = CreateEntryBlockAlloca("mpz", Type::MPZ);
	LLVMCompiler::value mpz_addr {Builder.CreateStructGEP(Type::LLVM_MPZ_TYPE, mpz_struct, 0), Type::POINTER};
	auto long_value = new_long(value);
	insn_call(Type::VOID, {mpz_addr, long_value}, &mpz_init_set_ui, "mpz_init_set_ui");
	// VM::inc_mpz_counter(F);
	return {mpz_struct, Type::MPZ_TMP};
}

LLVMCompiler::value LLVMCompiler::new_mpz_init(const mpz_t mpz) const {
	return new_mpz(0);
}

LLVMCompiler::value LLVMCompiler::new_array(Type element_type, std::vector<LLVMCompiler::value> elements) const {
	auto array = [&]() { if (element_type == Type::INTEGER) {
		return insn_call(Type::INT_ARRAY_TMP, {new_integer(elements.size())}, +[](int capacity) {
			auto array = new LSArray<int>();
			array->reserve(capacity);
			return array;
		}, "new_int_array");
	} else if (element_type == Type::REAL) {
		return insn_call(Type::REAL_ARRAY_TMP, {new_integer(elements.size())}, +[](int capacity) {
			auto array = new LSArray<double>();
			array->reserve(capacity);
			return array;
		}, "new_real_array");
	} else {
		return insn_call(Type::PTR_ARRAY_TMP, {new_integer(elements.size())}, +[](int capacity) {
			auto array = new LSArray<LSValue*>();
			array->reserve(capacity);
			return array;
		}, "new_ptr_array");
	}}();
	for (const auto& element : elements) {
		insn_push_array(array, element);
	}
	// size of the array + 1 operations
	inc_ops(elements.size() + 1);
	return array;
}

LLVMCompiler::value LLVMCompiler::to_int(LLVMCompiler::value v) const {
	if (v.t.not_temporary() == Type::MPZ) {
		auto v_addr = insn_address_of(v);
		return to_int(insn_call(Type::LONG, {v_addr}, &mpz_get_si, "mpz_get_si"));
	}
	LLVMCompiler::value r {Builder.CreateIntCast(v.v, Type::INTEGER.llvm_type(), true), Type::INTEGER};
	log_insn(4) << "to_int " << dump_val(v) << " " << dump_val(r) << std::endl;
	return r;
}
LLVMCompiler::value LLVMCompiler::to_real(LLVMCompiler::value) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::to_long(LLVMCompiler::value) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::insn_convert(LLVMCompiler::value, Type) const {
	assert(false);
}

// Operators wrapping
LLVMCompiler::value LLVMCompiler::insn_not(LLVMCompiler::value v) const {
	return {Builder.CreateNot(v.v), v.t};
}

LLVMCompiler::value LLVMCompiler::insn_not_bool(LLVMCompiler::value v) const {
	LLVMCompiler::value r {Builder.CreateNot(v.v), Type::BOOLEAN};
	log_insn(4) << "not_bool " << dump_val(v) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_neg(LLVMCompiler::value v) const {
	if (v.t == Type::REAL) {
		return {Builder.CreateFNeg(v.v), v.t};
	} else {
		return {Builder.CreateNeg(v.v), v.t};
	}
}

LLVMCompiler::value LLVMCompiler::insn_and(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateAnd(a.v, b.v), Type::INTEGER};
}
LLVMCompiler::value LLVMCompiler::insn_or(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateOr(a.v, b.v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_add(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateAdd(a.v, b.v, "addtmp"), Type::INTEGER};
}
LLVMCompiler::value LLVMCompiler::insn_sub(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateSub(a.v, b.v, "addtmp"), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_eq(LLVMCompiler::value a, LLVMCompiler::value b) const {
	LLVMCompiler::value r {Builder.CreateICmpEQ(a.v, b.v), Type::BOOLEAN};
	log_insn(4) << "eq " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_ne(LLVMCompiler::value a, LLVMCompiler::value b) const {
	LLVMCompiler::value r {Builder.CreateICmpNE(a.v, b.v), Type::BOOLEAN};
	log_insn(4) << "ne " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_lt(LLVMCompiler::value a, LLVMCompiler::value b) const {
	LLVMCompiler::value r {Builder.CreateICmpSLT(a.v, b.v), Type::BOOLEAN};
	log_insn(4) << "lt " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_le(LLVMCompiler::value a, LLVMCompiler::value b) const {
	LLVMCompiler::value r {Builder.CreateICmpSLE(a.v, b.v), Type::BOOLEAN};
	log_insn(4) << "le " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_gt(LLVMCompiler::value a, LLVMCompiler::value b) const {
	LLVMCompiler::value r {Builder.CreateICmpSGT(a.v, b.v), Type::BOOLEAN};
	log_insn(4) << "gt " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_ge(LLVMCompiler::value a, LLVMCompiler::value b) const {
	LLVMCompiler::value r {Builder.CreateICmpSGE(a.v, b.v), Type::BOOLEAN};
	log_insn(4) << "ge " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_mul(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateMul(a.v, b.v, "multmp"), Type::INTEGER};
}
LLVMCompiler::value LLVMCompiler::insn_div(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateFDiv(Builder.CreateFPCast(a.v, llvm::Type::getDoubleTy(context)), Builder.CreateFPCast(b.v, llvm::Type::getDoubleTy(context)), "divtmp"), Type::REAL};
}
LLVMCompiler::value LLVMCompiler::insn_int_div(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_bit_and(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }

LLVMCompiler::value LLVMCompiler::insn_bit_or(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateOr(a.v, b.v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_bit_xor(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_mod(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }

LLVMCompiler::value LLVMCompiler::insn_cmpl(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateICmpUGT(a.v, b.v), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_log(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_log10(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_ceil(LLVMCompiler::value) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::insn_round(LLVMCompiler::value) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::insn_floor(LLVMCompiler::value) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::insn_cos(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_sin(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_tan(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_acos(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_asin(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_atan(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_sqrt(LLVMCompiler::value) const { assert(false); }

LLVMCompiler::value LLVMCompiler::insn_pow(LLVMCompiler::value a, LLVMCompiler::value b) const {
	LLVMCompiler::value r = insn_call(Type::INTEGER, {a, b}, +[](int a, int b) {
		return (int) std::pow(a, b);
	});
	log_insn(4) << "pow " << dump_val(a) << " " << dump_val(b) << " " << dump_val(r) << std::endl;
	return r;
}

LLVMCompiler::value LLVMCompiler::insn_min(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_max(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_exp(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_atan2(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_abs(LLVMCompiler::value) const { assert(false); }

// LLVMCompiler::value management
LLVMCompiler::value LLVMCompiler::insn_create_value(Type t) const { assert(false); }

LLVMCompiler::value LLVMCompiler::insn_to_pointer(LLVMCompiler::value v) const {
	if (v.t.nature == Nature::POINTER) {
		return v; // already a pointer
	}
	Type new_type = v.t;
	new_type.nature = Nature::POINTER;
	if (v.t.raw_type == RawType::LONG) {
		return insn_call(new_type, {v}, +[](long n) {
			return LSNumber::get(n);
		}, "new_number");
	} else if (v.t.raw_type == RawType::REAL) {
		return insn_call(new_type, {v}, +[](double n) {
			return LSNumber::get(n);
		}, "new_number");
	} else if (v.t.raw_type == RawType::BOOLEAN) {
		return insn_call(new_type, {v}, +[](bool n) {
			return LSBoolean::get(n);
		}, "new_bool");
	} else {
		return insn_call(new_type, {v}, +[](int n) {
			return LSNumber::get(n);
		}, "new_number");
	}
}

LLVMCompiler::value LLVMCompiler::insn_to_bool(LLVMCompiler::value v) const {
	if (v.t.raw_type == RawType::BOOLEAN) {
		return v;
	}
	if (v.t.raw_type == RawType::INTEGER) {
		LLVMCompiler::value r {Builder.CreateICmpNE(v.v, llvm::Constant::getNullValue(v.v->getType()), "ifcond"), Type::BOOLEAN};
		log_insn(4) << "to_bool " << dump_val(v) << " " << dump_val(r) << std::endl;
		return r;
	}
	if (v.t.raw_type == RawType::STRING) {
		//return insn_call(Type::BOOLEAN, {v}, (void*) &LSString::to_bool, "String::to_bool");
	}
	if (v.t.raw_type == RawType::ARRAY) {
		// Always take LSArray<int>, but the array is not necessarily of this type
		//return insn_call(Type::BOOLEAN, {v}, (void*) &LSArray<int>::to_bool, "Array::to_bool");
	}
	if (v.t.raw_type == RawType::FUNCTION or v.t.raw_type == RawType::CLOSURE) {
		return new_bool(true);
	}
	if (v.t.raw_type == RawType::MPZ) {
		// TODO
		return v;
	}
	return insn_call(Type::BOOLEAN, {v}, +[](LSValue* v) {
		return v->to_bool();
	}, "Value::to_bool");
}

LLVMCompiler::value LLVMCompiler::insn_address_of(LLVMCompiler::value v) const { assert(false); }

LLVMCompiler::value LLVMCompiler::insn_load(LLVMCompiler::value v, int pos, Type t) const {
	LLVMCompiler::value r {Builder.CreateAlignedLoad(v.v, pos), t};
	log_insn(4) << "load " << dump_val(v) << " " << pos << " " << dump_val(r) << std::endl;
	return r;
}

void LLVMCompiler::insn_store(LLVMCompiler::value x, LLVMCompiler::value y) const {
	Builder.CreateStore(y.v, x.v);
	log_insn(4) << "store " << dump_val(x) << " " << dump_val(y) << std::endl;
}

void  LLVMCompiler::insn_store_relative(LLVMCompiler::value x, int pos, LLVMCompiler::value y) const {
	Builder.CreateAlignedStore(y.v, x.v, pos);
	log_insn(4) << "store_rel " << dump_val(x) << " " << dump_val(y) << std::endl;
}

LLVMCompiler::value LLVMCompiler::insn_typeof(LLVMCompiler::value v) const {
	if (v.t.raw_type == RawType::NULLL) return new_integer(LSValue::NULLL);
	if (v.t.raw_type == RawType::BOOLEAN) return new_integer(LSValue::BOOLEAN);
	if (v.t.isNumber()) return new_integer(LSValue::NUMBER);
	if (v.t.raw_type == RawType::STRING) return new_integer(LSValue::STRING);
	if (v.t.raw_type == RawType::ARRAY) return new_integer(LSValue::ARRAY);
	if (v.t.raw_type == RawType::MAP) return new_integer(LSValue::MAP);
	if (v.t.raw_type == RawType::SET) return new_integer(LSValue::SET);
	if (v.t.raw_type == RawType::INTERVAL) return new_integer(LSValue::INTERVAL);
	if (v.t.raw_type == RawType::FUNCTION) return new_integer(LSValue::FUNCTION);
	if (v.t.raw_type == RawType::CLOSURE) return new_integer(LSValue::CLOSURE);
	if (v.t.raw_type == RawType::OBJECT) return new_integer(LSValue::OBJECT);
	if (v.t.raw_type == RawType::CLASS) return new_integer(LSValue::CLASS);
	return insn_call(Type::INTEGER, {v}, +[](LSValue* v) {
		return v->type;
	}, "typeof");
}

LLVMCompiler::value LLVMCompiler::insn_class_of(LLVMCompiler::value v) const { assert(false); }

void LLVMCompiler::insn_delete(LLVMCompiler::value v) const {
	if (v.t.must_manage_memory()) {
		// insn_call(Type::VOID, {v}, (void*) &LSValue::delete_ref);
		insn_if_not(insn_native(v), [&]() {
			auto refs = insn_refs(v);
			insn_if(insn_refs(v), [&]() {
				insn_if_not(insn_dec_refs(v, refs), [&]() {
					insn_call(Type::VOID, {v}, (void*) &LSValue::free, "Value::free");
				});
			});
		});
	} else if (v.t.not_temporary() == Type::MPZ) {
		insn_delete_mpz(v);
	}
}

void LLVMCompiler::insn_delete_temporary(LLVMCompiler::value v) const {
	if (v.t.must_manage_memory()) {
		// insn_call(Type::VOID, {v}, (void*) &LSValue::delete_temporary);
		insn_if_not(insn_refs(v), [&]() {
			insn_call(Type::VOID, {v}, (void*) &LSValue::free, "Value::free");
		});
	} else if (v.t == Type::MPZ_TMP) {
		insn_delete_mpz(v);
	}
}

LLVMCompiler::value LLVMCompiler::insn_array_size(LLVMCompiler::value v) const {
	if (v.t.raw_type == RawType::STRING) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSString::int_size, "string_size");
	} else if (v.t.raw_type == RawType::ARRAY and v.t.getElementType() == Type::INTEGER) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<int>::int_size, "int_array_size");
	} else if (v.t.raw_type == RawType::ARRAY and v.t.getElementType() == Type::REAL) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<double>::int_size, "real_array_size");
	} else {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<LSValue*>::int_size, "ptr_array_size");
	}
	return {};
}

LLVMCompiler::value LLVMCompiler::insn_get_capture(int index, Type type) const { assert(false); }

void LLVMCompiler::insn_push_array(LLVMCompiler::value array, LLVMCompiler::value value) const {
	if (array.t.getElementType() == Type::INTEGER) {
		insn_call(Type::VOID, {array, value}, (void*) +[](LSArray<int>* array, int value) {
			array->push_back(value);
		}, "array_push_int");
	} else if (array.t.getElementType() == Type::REAL) {
		value.t = Type::REAL;
		insn_call(Type::VOID, {array, value}, (void*) +[](LSArray<double>* array, double value) {
			array->push_back(value);
		}, "array_push_real");
	} else {
		insn_call(Type::VOID, {array, value}, (void*) +[](LSArray<LSValue*>* array, LSValue* value) {
			array->push_inc(value);
		}, "array_push_ptr");
	}
}

LLVMCompiler::value LLVMCompiler::insn_array_at(LLVMCompiler::value array, LLVMCompiler::value index) const {
	auto converted_array = Builder.CreatePointerCast(array.v, Type::LLVM_VECTOR_TYPE_PTR);
	auto raw_data = Builder.CreateStructGEP(Type::LLVM_VECTOR_TYPE, converted_array, 3);
	auto data_base = Builder.CreateLoad(raw_data);
	auto data = Builder.CreatePointerCast(data_base, array.t.getElementType().llvm_type()->getPointerTo());
	return {Builder.CreateGEP(data, index.v), Type::POINTER};
}

LLVMCompiler::value LLVMCompiler::insn_move_inc(LLVMCompiler::value value) const {
	if (value.t.must_manage_memory()) {
		if (value.t.reference) {
			insn_inc_refs(value);
			return value;
		} else {
			return insn_call(value.t, {value}, (void*) +[](LSValue* v) {
				return v->move_inc();
			}, "move_inc");
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
	auto new_mpz = CreateEntryBlockAlloca("mpz", Type::MPZ);
	LLVMCompiler::value r = {new_mpz, Type::MPZ_TMP};
	LLVMCompiler::value r_addr {Builder.CreateStructGEP(Type::LLVM_MPZ_TYPE, new_mpz, 0), Type::POINTER};
	LLVMCompiler::value mpz_addr {Builder.CreateStructGEP(Type::LLVM_MPZ_TYPE, mpz.v, 0), Type::POINTER};
	insn_call(Type::VOID, {r_addr, mpz_addr}, &mpz_init_set, "mpz_init_set");
	// VM::inc_mpz_counter(F);
	return r;
}

void  LLVMCompiler::insn_delete_mpz(LLVMCompiler::value mpz) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_inc_refs(LLVMCompiler::value v) const { assert(false); }

LLVMCompiler::value LLVMCompiler::insn_dec_refs(LLVMCompiler::value v, LLVMCompiler::value previous) const {
	if (v.t.must_manage_memory()) {
		if (previous.v == nullptr) {
			previous = insn_refs(v);
		}
		auto new_refs = insn_sub(previous, new_integer(1));
		auto r = Builder.CreateStructGEP(Type::LLVM_LSVALUE_TYPE, v.v, 3);
		insn_store({r, Type::UNKNOWN}, new_refs);
		return new_refs;
	}
	return new_integer(0);
}

LLVMCompiler::value LLVMCompiler::insn_move(LLVMCompiler::value v) const {
	if (v.t.must_manage_memory() and !v.t.temporary and !v.t.reference) {
		return insn_call(v.t, {v}, (void*) +[](LSValue* v) {
			return v->move();
		}, "move");
	}
	return v;
}
LLVMCompiler::value LLVMCompiler::insn_refs(LLVMCompiler::value v) const {
	assert(v.t.must_manage_memory());
	auto r = Builder.CreateStructGEP(Type::LLVM_LSVALUE_TYPE, v.v, 3);
	return {Builder.CreateLoad(r), Type::INTEGER};
}
LLVMCompiler::value LLVMCompiler::insn_native(LLVMCompiler::value v) const {
	assert(v.t.must_manage_memory());
	auto r = Builder.CreateStructGEP(Type::LLVM_LSVALUE_TYPE, v.v, 4);
	return {Builder.CreateLoad(r), Type::BOOLEAN};
}

// Iterators
LLVMCompiler::value LLVMCompiler::iterator_begin(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::iterator_end(LLVMCompiler::value v, LLVMCompiler::value it) const { assert(false); }
LLVMCompiler::value LLVMCompiler::iterator_get(LLVMCompiler::value it, LLVMCompiler::value previous) const { assert(false); }
LLVMCompiler::value LLVMCompiler::iterator_key(LLVMCompiler::value v, LLVMCompiler::value it, LLVMCompiler::value previous) const { assert(false); }
void LLVMCompiler::iterator_increment(LLVMCompiler::value it) const { assert(false); }

// Controls
LLVMCompiler::label LLVMCompiler::insn_init_label(std::string name, llvm::Function* fun) const {
	return {llvm::BasicBlock::Create(LLVMCompiler::context, name, fun)};
}
void LLVMCompiler::insn_if(LLVMCompiler::value condition, std::function<void()> then) const {
	auto label_then = insn_init_label("then");
	auto label_end = insn_init_label("ifcont");
	insn_if_new(insn_to_bool(condition), &label_then, &label_end);
	insn_label(&label_then);
	then();
	insn_branch(&label_end);
	insn_label(&label_end);
}
void LLVMCompiler::insn_if_new(LLVMCompiler::value cond, label* then, label* elze) const {
	Builder.CreateCondBr(cond.v, then->block, elze->block);
}

void LLVMCompiler::insn_if_not(LLVMCompiler::value condition, std::function<void()> then) const {
	auto label_then = insn_init_label("then");
	auto label_end = insn_init_label("ifcont");
	insn_if_new(insn_to_bool(condition), &label_end, &label_then);
	insn_label(&label_then);
	then();
	insn_branch(&label_end);
	insn_label(&label_end);
}

void LLVMCompiler::insn_throw(LLVMCompiler::value v) const {
	std::cout << "TODO exception" << std::endl;
}
void LLVMCompiler::insn_throw_object(vm::Exception type) const {
	std::cout << "TODO exception" << std::endl;
}

void LLVMCompiler::insn_label(label* l) const {
	auto function = LLVMCompiler::Builder.GetInsertBlock()->getParent();
	function->getBasicBlockList().push_back(l->block);
	Builder.SetInsertPoint(l->block);
}

void LLVMCompiler::insn_branch(label* l) const {
	Builder.CreateBr(l->block);
}

void LLVMCompiler::insn_branch_if(LLVMCompiler::value v, label* l) const {
	assert(false);
}

void LLVMCompiler::insn_branch_if_not(LLVMCompiler::value v, label* l) const { assert(false); }
void LLVMCompiler::insn_branch_if_pc_not_in_range(label* a, label* b, label* n) const { assert(false); }
void LLVMCompiler::insn_return(LLVMCompiler::value v) const {
	LLVMCompiler::Builder.CreateRet(v.v);
}

// Call functions
LLVMCompiler::value LLVMCompiler::insn_call(Type return_type, std::vector<LLVMCompiler::value> args, void* func, std::string function_name) const {
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t.llvm_type());
	}
	if (!function_name.size()) function_name = std::string("anonymous_func_") + std::to_string(mappings.size());
	auto i = mappings.find(function_name);
	if (i == mappings.end()) {
		auto fun_type = llvm::FunctionType::get(return_type.llvm_type(), llvm_types, false);
		auto lambdaFN = llvm::Function::Create(fun_type, llvm::Function::ExternalLinkage, function_name, LLVMCompiler::TheModule.get());
		((LLVMCompiler*) this)->mappings.insert({function_name, {(llvm::JITTargetAddress) func, lambdaFN}});
	}
	return {Builder.CreateCall(mappings.at(function_name).function, llvm_args, function_name), return_type};
}

LLVMCompiler::value LLVMCompiler::insn_call_indirect(Type return_type, LLVMCompiler::value fun, std::vector<LLVMCompiler::value> args) const {
	return new_integer(0);
}

void LLVMCompiler::function_add_capture(LLVMCompiler::value fun, LLVMCompiler::value capture) { assert(false); }
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
			LLVMCompiler::value v = {LLVMCompiler::Builder.CreateLoad(it->second.v, it->first.c_str()), it->second.t};
			insn_delete(v);
		}
	}
}

void LLVMCompiler::enter_function(llvm::Function* F, bool is_closure, Function* fun) {
	variables.push_back(std::map<std::string, value> {});
	function_variables.push_back(std::vector<value> {});
	functions.push(F);
	functions_blocks.push_back(0);
	catchers.push_back({});
	function_is_closure.push(is_closure);

	std::vector<std::string> args;
	log_insn(0) << "function " << fun->name << "(";
	for (unsigned i = 0; i < fun->arguments.size(); ++i) {
		log_insn(0) << fun->arguments.at(i)->content;
		if (i < fun->arguments.size() - 1) log_insn(0) << ", ";
		args.push_back(fun->arguments.at(i)->content);
	}
	arg_names.push(args);
	log_insn(0) << ") {" << std::endl;
}

void LLVMCompiler::leave_function() {
	variables.pop_back();
	function_variables.pop_back();
	functions.pop();
	functions_blocks.pop_back();
	catchers.pop_back();
	function_is_closure.pop();
	arg_names.pop();
	this->F = functions.top();
	log_insn(0) << "}" << std::endl;
}

int LLVMCompiler::get_current_function_blocks() const {
	return 0; // TODO
}
void LLVMCompiler::delete_function_variables() {
	// TODO
}
bool LLVMCompiler::is_current_function_closure() const { assert(false); }

// Variables
void LLVMCompiler::add_var(const std::string& name, LLVMCompiler::value value) {
	assert((value.v != nullptr) && "value must not be null");
	variables.back()[name] = value;
	var_map.insert({value.v, name});
}

LLVMCompiler::value LLVMCompiler::create_and_add_var(const std::string& name, Type type) {
	// std::cout << "create var " << name << " type " << type << " / " << type.llvm_type() << std::endl;
	auto value = CreateEntryBlockAlloca(name, type);
	LLVMCompiler::value v { value, type };
	// std::cout << "Var " << name << " created with type " << v.v->getType() << std::endl;
	variables.back()[name] = v;
	var_map.insert({value, name});
	return v;
}

void LLVMCompiler::add_function_var(LLVMCompiler::value value) {
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

std::map<std::string, LLVMCompiler::value> LLVMCompiler::get_vars() { assert(false); }
void LLVMCompiler::update_var(std::string& name, LLVMCompiler::value) { assert(false); }


LLVMCompiler::value LLVMCompiler::update_var_create(std::string& name, Type type) {
	auto value = CreateEntryBlockAlloca(name, type);
	LLVMCompiler::value v { value, type };
	variables.back()[name] = v;
	var_map.insert({value, name});
	return v;
}

// Loops
void LLVMCompiler::enter_loop(LLVMCompiler::label*, LLVMCompiler::label*) { assert(false); }
void LLVMCompiler::leave_loop() { assert(false); }
LLVMCompiler::label* LLVMCompiler::get_current_loop_end_label(int deepness) const { assert(false); }
LLVMCompiler::label* LLVMCompiler::get_current_loop_cond_label(int deepness) const { assert(false); }
int LLVMCompiler::get_current_loop_blocks(int deepness) const { assert(false); }

/** Operations **/
void LLVMCompiler::inc_ops(int add) const {
	// TODO
}
void LLVMCompiler::inc_ops_jit(LLVMCompiler::value add) const { assert(false); }
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
		auto type = types[i];
		if (arg.t.nature != Nature::VALUE and type != arg.t.id() and type != 0) {
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
	// TODO
}
void LLVMCompiler::add_literal(void* ptr, std::string value) const {
	((LLVMCompiler*) this)->literals.insert({ptr, value});
}

}
