#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <cassert>
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

namespace ls {

class Base_type;
class Function;
class Value;
class Compiler;

class Type {

	Type();
	Type(std::set<std::shared_ptr<const Base_type>>, const Type* folded);
	Type(std::shared_ptr<const Base_type> raw_type, bool native = false);

public:
	std::vector<std::shared_ptr<const Base_type>> _types;
	const Type* folded;
	bool native = false; // A C++ object, memory management is done outside the language
	bool temporary = false;
	bool constant = false;
	bool reference = false;

	int id() const;
	const Type* return_type() const;
	const Type* argument(size_t index) const;
	const std::vector<const Type*>& arguments() const;
	const Type* element() const;
	const Type* key() const;
	const Type* member(int i) const;

	void toJson(std::ostream&) const;
	std::string getJsonName() const;
	std::string to_string() const;
	std::string class_name() const;
	bool must_manage_memory() const;
	bool iterable() const;
	bool is_container() const;
	bool can_be_container() const;
	const Type* add_temporary() const;
	const Type* not_temporary() const;
	const Type* add_constant() const;
	const Type* not_constant() const;
	llvm::Type* llvm_type(const Compiler& c) const;
	const Type* iterator() const;
	const Type* pointer() const;
	const Type* pointed() const;
	bool all(std::function<bool(std::shared_ptr<const Base_type>)>) const;
	bool some(std::function<bool(std::shared_ptr<const Base_type>)>) const;
	bool castable(const Type* type, bool strictCast = false) const;
	bool strictCastable(const Type* type) const;
	int distance(const Type* type) const;
	const Type* without_placeholders() const;

	const Type* operator + (std::shared_ptr<const Base_type>) const;
	const Type* operator + (const Type* type) const;
	void operator += (const Type* type);
	void operator += (std::shared_ptr<const Base_type> type);
	const Type* operator * (const Type* t2) const;
	const Type* fold() const;

	template <class T> bool is_type() const;
	template <class T> bool can_be_type() const;
	bool is_any() const;
	bool is_number() const;
	bool can_be_number() const;
	bool can_be_numeric() const;
	bool is_integer() const;
	bool is_bool() const;
	bool can_be_bool() const;
	bool is_long() const;
	bool is_real() const;
	bool is_mpz() const;
	bool is_mpz_ptr() const;
	bool is_array() const;
	bool is_string() const;
	bool is_set() const;
	bool is_interval() const;
	bool is_map() const;
	bool is_closure() const;
	bool is_function() const;
	bool is_object() const;
	bool is_never() const;
	bool is_null() const;
	bool is_class() const;
	bool is_placeholder() const;
	bool is_pointer() const;
	bool is_struct() const;
	bool is_polymorphic() const;
	bool is_primitive() const;
	bool is_callable() const;
	bool can_be_callable() const;
	bool is_void() const;
	bool is_template() const;

	void implement(const Type* type) const;
	bool is_implemented(const Type* type) const;

	/*
	 * Static part
	 */
	static unsigned int placeholder_counter;

	static const Type* const void_;
	static const Type* const never;
	static const Type* const any;
	static const Type* const null;
	static const Type* const const_any;
	static const Type* const boolean;
	static const Type* const const_boolean;
	static const Type* const number;
	static const Type* const const_number;
	static const Type* const i8;
	static const Type* const integer;
	static const Type* const const_integer;
	static const Type* const long_;
	static const Type* const const_long;
	static const Type* const mpz;
	static const Type* const tmp_mpz;
	static const Type* const const_mpz;
	static const Type* const mpz_ptr;
	static const Type* const tmp_mpz_ptr;
	static const Type* const tmp_string;
	static const Type* const const_string;
	static const Type* const real;
	static const Type* const const_real;
	static const Type* const string;
	static const Type* array(const Type* = Type::void_);
	static const Type* const_array(const Type* = Type::void_);
	static const Type* tmp_array(const Type* = Type::void_);
	static const Type* const object;
	static const Type* const tmp_object;
	static const Type* set(const Type* = Type::void_);
	static const Type* const_set(const Type* = Type::void_);
	static const Type* tmp_set(const Type* = Type::void_);
	static const Type* map(const Type* = Type::void_, const Type* = Type::void_);
	static const Type* tmp_map(const Type* = Type::void_, const Type* = Type::void_);
	static const Type* const_map(const Type* = Type::void_, const Type* = Type::void_);
	static const Type* const interval;
	static const Type* const const_interval;
	static const Type* const tmp_interval;
	static const Type* fun(const Type* return_type = Type::void_, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	static const Type* closure(const Type* return_type = Type::void_, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	static const Type* structure(const std::string name, std::initializer_list<const Type*> types);
	static const Type* clazz(const std::string name = "class?");
	static const Type* const_class(const std::string name = "class?");
	static const Type* template_(std::string name);
	static const Type* compound(std::initializer_list<const Type*> types);

	static std::shared_ptr<Base_type> _raw_never;
	static std::shared_ptr<Base_type> _raw_null;
	static std::shared_ptr<Base_type> _raw_any;
	static std::shared_ptr<Base_type> _raw_boolean;
	static std::shared_ptr<Base_type> _raw_i8;
	static std::shared_ptr<Base_type> _raw_integer;
	static std::shared_ptr<Base_type> _raw_number;
	static std::shared_ptr<Base_type> _raw_long;
	static std::shared_ptr<Base_type> _raw_real;
	static std::shared_ptr<Base_type> _raw_string;
	static std::shared_ptr<Base_type> _raw_mpz;
	static std::shared_ptr<Base_type> _raw_interval;
	static std::shared_ptr<Base_type> _raw_object;
	static std::map<std::string, std::shared_ptr<Base_type>> _raw_class;
	static const std::shared_ptr<Base_type> raw_never();
	static const std::shared_ptr<Base_type> raw_null();
	static const std::shared_ptr<Base_type> raw_any();
	static const std::shared_ptr<Base_type> raw_boolean();
	static const std::shared_ptr<Base_type> raw_i8();
	static const std::shared_ptr<Base_type> raw_integer();
	static const std::shared_ptr<Base_type> raw_number();
	static const std::shared_ptr<Base_type> raw_long();
	static const std::shared_ptr<Base_type> raw_real();
	static const std::shared_ptr<Base_type> raw_mpz();
	static const std::shared_ptr<Base_type> raw_string();
	static const std::shared_ptr<Base_type> raw_interval();
	static const std::shared_ptr<Base_type> raw_object();
	
	static const Type* generate_new_placeholder_type();
	static std::vector<std::shared_ptr<const Base_type>> placeholder_types;
	static void clear_placeholder_types();

	// Const types to be used to optimize return of references
	static const std::vector<const Type*> empty_types;
	static std::map<std::set<std::shared_ptr<const Base_type>>, const Type*> compound_types;
	static std::map<const Type*, const Type*> array_types;
	static std::map<const Type*, const Type*> const_array_types;
	static std::map<const Type*, const Type*> tmp_array_types;
	static std::map<const Type*, const Type*> set_types;
	static std::map<const Type*, const Type*> const_set_types;
	static std::map<const Type*, const Type*> tmp_set_types;
	static std::map<std::pair<const Type*, const Type*>, const Type*> map_types;
	static std::map<std::pair<const Type*, const Type*>, const Type*> const_map_types;
	static std::map<std::pair<const Type*, const Type*>, const Type*> tmp_map_types;
	static std::map<const Type*, const Type*> pointer_types;
	static std::map<const Type*, const Type*> temporary_types;
	static std::map<const Type*, const Type*> not_temporary_types;
	static std::map<const Type*, const Type*> const_types;
	static std::map<const Type*, const Type*> not_const_types;
};

std::ostream& operator << (std::ostream&, const Type*);
std::ostream& operator << (std::ostream&, const std::vector<const Type*>&);
std::ostream& operator << (std::ostream&, const llvm::Type*);

}

#endif
