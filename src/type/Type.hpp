#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <iostream>
#include <cassert>
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

namespace ls {

class BaseRawType;

class Type {
public:
	std::vector<const BaseRawType*> _types;
	bool native; // A C++ object, memory management is done outside the language
	std::vector<Type> return_types;
	std::vector<Type> arguments_types;
	std::vector<bool> arguments_has_default;
	bool pointer = false;
	bool temporary = false;
	bool constant = false;
	bool reference = false;

	Type();
	Type(const BaseRawType* raw_type, bool native = false, bool temporary = false, bool constant = false);

	int id() const;
	const BaseRawType* raw() const;

	bool must_manage_memory() const;

	Type getReturnType() const;
	void setReturnType(Type type);

	void addArgumentType(Type type);
	void setArgumentType(size_t index, Type type, bool has_default = false);
	const Type getArgumentType(size_t index) const;
	const std::vector<Type> getArgumentTypes() const;
	bool argumentHasDefault(size_t index) const;

	const Type getElementType() const;
	void setElementType(const Type&);
	const Type getKeyType() const;

	bool will_take(const std::vector<Type>& args_type);

	void add(const Type type);
	void add(const BaseRawType* type);

	void toJson(std::ostream&) const;
	std::string getJsonName() const;
	std::string to_string() const;
	std::string getClass() const;

	bool isNumber() const;
	bool iterable() const;
	bool is_container() const;
	int size() const;
	Type not_temporary() const;
	Type add_temporary() const;
	llvm::Type* llvm_type() const;
	Type add_pointer() const;
	Type iteratorType() const;

	bool is_array() const;
	bool is_set() const;
	bool is_interval() const;
	bool is_map() const;

	bool operator ==(const Type& type) const;
	inline bool operator !=(const Type& type) const { return !(*this == type); }

	bool operator < (const Type& type) const;

	bool compatible(const Type& type) const;
	bool may_be_compatible(const Type& type) const;

	/*
	 * Static part
	 */
	static unsigned int placeholder_counter;

	static const Type VALUE;
	static const Type CONST_VALUE;
	static const Type POINTER;
	static const Type CONST_POINTER;

	static const Type NULLL;
	static const Type BOOLEAN;
	static const Type CONST_BOOLEAN;
	static const Type NUMBER;
	static const Type CONST_NUMBER;
	static const Type INTEGER;
	static const Type CONST_INTEGER;
	static const Type MPZ;
	static const Type MPZ_TMP;
	static const Type LONG;
	static const Type CONST_LONG;
	static const Type REAL;
	static const Type CONST_REAL;
	static const Type STRING;
	static const Type CONST_STRING;
	static const Type STRING_TMP;
	static const Type OBJECT;
	static const Type OBJECT_TMP;

	// Arrays
	static const Type ARRAY;
	static const Type CONST_ARRAY;
	static const Type PTR_ARRAY;
	static const Type INT_ARRAY;
	static const Type REAL_ARRAY;
	static const Type PTR_ARRAY_TMP;
	static const Type INT_ARRAY_TMP;
	static const Type REAL_ARRAY_TMP;
	static const Type STRING_ARRAY;
	static const Type CONST_PTR_ARRAY;
	static const Type CONST_INT_ARRAY;
	static const Type CONST_REAL_ARRAY;
	static const Type CONST_STRING_ARRAY;
	static const Type PTR_ARRAY_ARRAY;
	static const Type REAL_ARRAY_ARRAY;
	static const Type INT_ARRAY_ARRAY;

	static const Type MAP;
	static const Type CONST_MAP;
	static const Type PTR_PTR_MAP;
	static const Type PTR_INT_MAP;
	static const Type PTR_REAL_MAP;
	static const Type REAL_PTR_MAP;
	static const Type REAL_INT_MAP;
	static const Type REAL_REAL_MAP;
	static const Type INT_PTR_MAP;
	static const Type INT_INT_MAP;
	static const Type INT_REAL_MAP;
	static const Type CONST_PTR_PTR_MAP;
	static const Type CONST_PTR_INT_MAP;
	static const Type CONST_PTR_REAL_MAP;
	static const Type CONST_REAL_PTR_MAP;
	static const Type CONST_REAL_INT_MAP;
	static const Type CONST_REAL_REAL_MAP;
	static const Type CONST_INT_PTR_MAP;
	static const Type CONST_INT_INT_MAP;
	static const Type CONST_INT_REAL_MAP;
	static const Type SET;
	static const Type PTR_SET;
	static const Type INT_SET;
	static const Type REAL_SET;
	static const Type CONST_SET;
	static const Type CONST_INT_SET;
	static const Type CONST_REAL_SET;
	static const Type INTERVAL;
	static const Type INTERVAL_TMP;
	static const Type FUNCTION;
	static const Type CONST_FUNCTION;
	static const Type FUNCTION_P;
	static const Type CONST_FUNCTION_P;
	static const Type CLOSURE;
	static const Type CLASS;
	static const Type CONST_CLASS;

	static const Type INT_ARRAY_ITERATOR;
	static const Type REAL_ARRAY_ITERATOR;
	static const Type PTR_ARRAY_ITERATOR;
	static const Type STRING_ITERATOR;
	static const Type INTERVAL_ITERATOR;
	static const Type SET_ITERATOR;
	static const Type INTEGER_ITERATOR;
	static const Type LONG_ITERATOR;
	static const Type MPZ_ITERATOR;

	static llvm::StructType* LLVM_LSVALUE_TYPE;
	static llvm::Type* LLVM_LSVALUE_TYPE_PTR;
	static llvm::Type* LLVM_LSVALUE_TYPE_PTR_PTR;
	static llvm::Type* LLVM_MPZ_TYPE;
	static llvm::Type* LLVM_MPZ_TYPE_PTR;
	static llvm::StructType* LLVM_VECTOR_TYPE;
	static llvm::Type* LLVM_VECTOR_TYPE_PTR;
	static llvm::StructType* LLVM_VECTOR_INT_TYPE;
	static llvm::Type* LLVM_VECTOR_INT_TYPE_PTR;
	static llvm::StructType* LLVM_VECTOR_REAL_TYPE;
	static llvm::Type* LLVM_VECTOR_REAL_TYPE_PTR;
	static llvm::StructType* LLVM_FUNCTION_TYPE;
	static llvm::Type* LLVM_FUNCTION_TYPE_PTR;
	static llvm::StructType* LLVM_INTEGER_ITERATOR_TYPE;

	static Type array(const Type = {});
	static Type const_array(const Type = {});
	static Type tmp_array(const Type = {});
	static Type set(const Type = {});
	static Type const_set(const Type = {});
	static Type tmp_set(const Type = {});
	static Type map(const Type = {}, const Type = {});
	static Type const_map(const Type = {}, const Type = {});
	static Type interval();
	static Type tmp_interval();
	static Type fun();
	static Type iterator(const Type);

	static bool list_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual);
	static bool list_may_be_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual);
	static bool list_more_specific(const std::vector<Type>& old, const std::vector<Type>& neww);
	static bool more_specific(const Type& old, const Type& neww);
	static Type get_compatible_type(const Type& t1, const Type& t2);
	static Type generate_new_placeholder_type();
};

std::ostream& operator << (std::ostream&, const Type&);
std::ostream& operator << (std::ostream&, const std::vector<Type>&);
std::ostream& operator << (std::ostream&, const llvm::Type*);

}

#endif
