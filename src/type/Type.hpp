#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <map>
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
public:
	std::vector<std::shared_ptr<const Base_type>> _types;
	bool native; // A C++ object, memory management is done outside the language
	bool temporary = false;
	bool constant = false;
	bool reference = false;

	Type();
	Type(std::initializer_list<std::shared_ptr<const Base_type>>);
	Type(std::initializer_list<Type> types);
	Type(std::shared_ptr<const Base_type> raw_type, bool native = false, bool temporary = false, bool constant = false);

	int id() const;
	const Type& return_type() const;
	const Type& argument(size_t index) const;
	const std::vector<Type>& arguments() const;
	const Type& element() const;
	const Type& key() const;
	const Type& member(int i) const;

	void toJson(std::ostream&) const;
	std::string getJsonName() const;
	std::string to_string() const;
	std::string class_name() const;
	bool must_manage_memory() const;
	bool iterable() const;
	bool is_container() const;
	bool can_be_container() const;
	Type not_temporary() const;
	llvm::Type* llvm_type(const Compiler& c) const;
	Type iterator() const;
	Type pointer() const;
	Type pointed() const;
	bool all(std::function<bool(std::shared_ptr<const Base_type>)>) const;
	bool some(std::function<bool(std::shared_ptr<const Base_type>)>) const;
	bool castable(Type type, bool strictCast = false) const;
	bool strictCastable(Type type) const;
	int distance(Type type) const;

	void operator += (const Type type);
	void operator += (std::shared_ptr<const Base_type> type);
	Type operator * (const Type& t2) const;
	Type fold() const;

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

	bool operator ==(const Type& type) const;
	inline bool operator !=(const Type& type) const { return !(*this == type); }

	bool operator < (const Type& type) const;

	bool compatible(const Type& type) const;
	bool may_be_compatible(const Type& type) const;

	void implement(const Type& type) const;
	bool is_implemented(const Type& type) const;

	/*
	 * Static part
	 */
	static unsigned int placeholder_counter;

	static Type never();
	static Type null();
	static Type any();
	static Type const_any();
	static Type boolean();
	static Type const_boolean();
	static Type number();
	static Type const_number();
	static Type i8();
	static Type integer();
	static Type const_integer();
	static Type long_();
	static Type const_long();
	static Type mpz();
	static Type tmp_mpz();
	static Type const_mpz();
	static Type mpz_ptr();
	static Type tmp_mpz_ptr();
	static Type string();
	static Type tmp_string();
	static Type const_string();
	static Type real();
	static Type const_real();
	static Type array(const Type = {});
	static Type const_array(const Type = {});
	static Type tmp_array(const Type = {});
	static Type object();
	static Type tmp_object();
	static Type set(const Type = {});
	static Type const_set(const Type = {});
	static Type tmp_set(const Type = {});
	static Type map(const Type = {}, const Type = {});
	static Type tmp_map(const Type = {}, const Type = {});
	static Type const_map(const Type = {}, const Type = {});
	static Type interval();
	static Type const_interval();
	static Type tmp_interval();
	static Type fun(Type return_type = {}, std::vector<Type> arguments = {}, const Value* function = nullptr);
	static Type closure(Type return_type = {}, std::vector<Type> arguments = {}, const Value* function = nullptr);
	static Type structure(const std::string name, std::initializer_list<Type> types);
	static Type clazz(const std::string name = "class?");
	static Type const_class(const std::string name = "class?");
	static Type template_(std::string name);

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
	static std::shared_ptr<Base_type> _raw_mpz_ptr;
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
	static const std::shared_ptr<Base_type> raw_mpz_ptr();
	static const std::shared_ptr<Base_type> raw_string();
	static const std::shared_ptr<Base_type> raw_interval();
	static const std::shared_ptr<Base_type> raw_object();
	
	static std::shared_ptr<const Base_type> generate_new_placeholder_type();
	static std::vector<std::shared_ptr<const Base_type>> placeholder_types;
	static void clear_placeholder_types();

	// Const types to be used to optimize return of references
	static const std::vector<Type> empty_types;
	static const Type void_type;
	static const Type any_type;
	static const Type integer_type;
	static const Type string_type;
};

std::ostream& operator << (std::ostream&, const Type&);
std::ostream& operator << (std::ostream&, const std::vector<Type>&);
std::ostream& operator << (std::ostream&, const llvm::Type*);

}

#endif
