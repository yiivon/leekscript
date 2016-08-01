/*
 * Describe an array in LeekScript
 */
#ifndef LS_ARRAY_BASE
#define LS_ARRAY_BASE

#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <string>

#include "../LSValue.hpp"
#include "../../../lib/json.hpp"
#include "LSClass.hpp"

namespace ls {

template <typename T>
using LSArrayIterator = typename std::vector<T>::iterator;

template <typename T> class LSArray : public LSValue, public std::vector<T> {
public:

	static LSValue* array_class;

	LSArray();
	LSArray(std::initializer_list<T>);
	LSArray(const std::vector<T>&);
	LSArray(Json& data);

	virtual ~LSArray();

	/*
	 * Array functions
	 */
	void push_no_clone(T value);
	void push_clone(const T value);
	void clear();
	T remove(const int index);
	T remove_key(LSValue* key);
	T remove_element(T element);
	LSValue* pop();
	virtual size_t size() const;
	virtual T sum() const;
	virtual double average() const;
	virtual T first() const;
	virtual T last() const;
	int atv(const int i);
	int* atLv(int i);
	LSArray<LSValue*>* map(const void*) const;
	LSArray<LSArray<T>*>* chunk(int size = 1) const;
	LSArray<LSArray<T>*>* chunk_1() const;
	void unique();
	void sort();
	void iter(const LSFunction*) const;
	int contains(const LSValue*) const;
	int contains_int(int) const;
	LSArray<T>* push(const T val);
	LSArray<T>* push_all(const LSArray<LSValue*>*);
	const LSArray<T>* push_all_int(const LSArray<int>*);
	LSArray<T>* reverse() const;
	LSArray<T>* filter(const void* fun) const;
	LSValue* foldLeft(const void* fun, const LSValue* initial) const;
	LSValue* foldRight(const void* fun, const LSValue* initial) const;
	LSArray<T>* insert_v(const T v, const LSValue* pos);
	LSArray<LSArray<T>*>* partition(const void* fun) const;
	LSArray<LSValue*>* map2(const LSArray<LSValue*>*, const void* fun) const;
	LSArray<LSValue*>* map2_int(const LSArray<int>*, const void* fun) const;
	int search(const LSValue* search, const int start) const;
	int search_int(const int search, const int start) const;
	LSString* join(const LSString* glue) const;
	LSArray<T>* fill(const LSValue* element, const int size);

	/*
	 * LSValue methods
	 */
	bool isTrue() const override;

	LSValue* operator - () const override;
	LSValue* operator ! () const override;
	LSValue* operator ~ () const override;

	LSValue* operator ++ () override;
	LSValue* operator ++ (int) override;

	LSValue* operator -- () override;
	LSValue* operator -- (int) override;

	LSValue* operator + (const LSValue*) const override;
	LSValue* operator + (const LSNull*) const override;
	LSValue* operator + (const LSBoolean*) const override;
	LSValue* operator + (const LSNumber*) const override;
	LSValue* operator + (const LSString*) const override;
	LSValue* operator + (const LSArray<LSValue*>*) const override;
	LSValue* operator + (const LSArray<int>*) const override;
	LSValue* operator + (const LSObject*) const override;
	LSValue* operator + (const LSFunction*) const override;
	LSValue* operator + (const LSClass*) const override;

	LSValue* operator += (LSValue*) override;
	LSValue* operator += (const LSNull*) override;
	LSValue* operator += (const LSBoolean*) override;
	LSValue* operator += (const LSNumber*) override;
	LSValue* operator += (const LSString*) override;
	LSValue* operator += (const LSArray<LSValue*>*) override;
	LSValue* operator += (const LSObject*) override;
	LSValue* operator += (const LSFunction*) override;
	LSValue* operator += (const LSClass*) override;

	LSValue* operator - (const LSValue*) const override;
	LSValue* operator - (const LSNull*) const override;
	LSValue* operator - (const LSBoolean*) const override;
	LSValue* operator - (const LSNumber*) const override;
	LSValue* operator - (const LSString*) const override;
	LSValue* operator - (const LSArray<LSValue*>*) const override;
	LSValue* operator - (const LSObject*) const override;
	LSValue* operator - (const LSFunction*) const override;
	LSValue* operator - (const LSClass*) const override;

	LSValue* operator -= (LSValue*) override;
	LSValue* operator -= (const LSNull*) override;
	LSValue* operator -= (const LSBoolean*) override;
	LSValue* operator -= (const LSNumber*) override;
	LSValue* operator -= (const LSString*) override;
	LSValue* operator -= (const LSArray<LSValue*>*) override;
	LSValue* operator -= (const LSObject*) override;
	LSValue* operator -= (const LSFunction*) override;
	LSValue* operator -= (const LSClass*) override;

	LSValue* operator * (const LSValue*) const override;
	LSValue* operator * (const LSNull*) const override;
	LSValue* operator * (const LSBoolean*) const override;
	LSValue* operator * (const LSNumber*) const override;
	LSValue* operator * (const LSString*) const override;
	LSValue* operator * (const LSArray<LSValue*>*) const override;
	LSValue* operator * (const LSObject*) const override;
	LSValue* operator * (const LSFunction*) const override;
	LSValue* operator * (const LSClass*) const override;

	LSValue* operator *= (LSValue*) override;
	LSValue* operator *= (const LSNull*) override;
	LSValue* operator *= (const LSBoolean*) override;
	LSValue* operator *= (const LSNumber*) override;
	LSValue* operator *= (const LSString*) override;
	LSValue* operator *= (const LSArray<LSValue*>*) override;
	LSValue* operator *= (const LSObject*) override;
	LSValue* operator *= (const LSFunction*) override;
	LSValue* operator *= (const LSClass*) override;

	LSValue* operator / (const LSValue*) const override;
	LSValue* operator / (const LSNull*) const override;
	LSValue* operator / (const LSBoolean*) const override;
	LSValue* operator / (const LSNumber*) const override;
	LSValue* operator / (const LSString*) const override;
	LSValue* operator / (const LSArray<LSValue*>*) const override;
	LSValue* operator / (const LSObject*) const override;
	LSValue* operator / (const LSFunction*) const override;
	LSValue* operator / (const LSClass*) const override;

	LSValue* operator /= (LSValue*) override;
	LSValue* operator /= (const LSNull*) override;
	LSValue* operator /= (const LSBoolean*) override;
	LSValue* operator /= (const LSNumber*) override;
	LSValue* operator /= (const LSString*) override;
	LSValue* operator /= (const LSArray<LSValue*>*) override;
	LSValue* operator /= (const LSObject*) override;
	LSValue* operator /= (const LSFunction*) override;
	LSValue* operator /= (const LSClass*) override;

	LSValue* poww(const LSValue*) const override;
	LSValue* poww(const LSNull*) const override;
	LSValue* poww(const LSBoolean*) const override;
	LSValue* poww(const LSNumber*) const override;
	LSValue* poww(const LSString*) const override;
	LSValue* poww(const LSArray<LSValue*>*) const override;
	LSValue* poww(const LSObject*) const override;
	LSValue* poww(const LSFunction*) const override;
	LSValue* poww(const LSClass*) const override;

	LSValue* pow_eq(LSValue*) override;
	LSValue* pow_eq(const LSNull*) override;
	LSValue* pow_eq(const LSBoolean*) override;
	LSValue* pow_eq(const LSNumber*) override;
	LSValue* pow_eq(const LSString*) override;
	LSValue* pow_eq(const LSArray<LSValue*>*) override;
	LSValue* pow_eq(const LSObject*) override;
	LSValue* pow_eq(const LSFunction*) override;
	LSValue* pow_eq(const LSClass*) override;

	LSValue* operator % (const LSValue*) const override;
	LSValue* operator % (const LSNull*) const override;
	LSValue* operator % (const LSBoolean*) const override;
	LSValue* operator % (const LSNumber*) const override;
	LSValue* operator % (const LSString*) const override;
	LSValue* operator % (const LSArray<LSValue*>*) const override;
	LSValue* operator % (const LSObject*) const override;
	LSValue* operator % (const LSFunction*) const override;
	LSValue* operator % (const LSClass*) const override;

	LSValue* operator %= (LSValue*) override;
	LSValue* operator %= (const LSNull*) override;
	LSValue* operator %= (const LSBoolean*) override;
	LSValue* operator %= (const LSNumber*) override;
	LSValue* operator %= (const LSString*) override;
	LSValue* operator %= (const LSArray<LSValue*>*) override;
	LSValue* operator %= (const LSObject*) override;
	LSValue* operator %= (const LSFunction*) override;
	LSValue* operator %= (const LSClass*) override;

	bool operator == (const LSValue*) const override;
	bool operator == (const LSNull*) const override;
	bool operator == (const LSBoolean*) const override;
	bool operator == (const LSNumber*) const override;
	bool operator == (const LSString*) const override;
	bool operator == (const LSArray<LSValue*>*) const override;
	bool operator == (const LSFunction*) const override;
	bool operator == (const LSObject*) const override;
	bool operator == (const LSClass*) const override;

	bool operator < (const LSValue*) const override;
	bool operator < (const LSNull*) const override;
	bool operator < (const LSBoolean*) const override;
	bool operator < (const LSNumber*) const override;
	bool operator < (const LSString*) const override;
	bool operator < (const LSArray<LSValue*>*) const override;
	bool operator < (const LSFunction*) const override;
	bool operator < (const LSObject*) const override;
	bool operator < (const LSClass*) const override;

	bool operator > (const LSValue*) const override;
	bool operator > (const LSNull*) const override;
	bool operator > (const LSBoolean*) const override;
	bool operator > (const LSNumber*) const override;
	bool operator > (const LSString*) const override;
	bool operator > (const LSArray<LSValue*>*) const override;
	bool operator > (const LSFunction*) const override;
	bool operator > (const LSObject*) const override;
	bool operator > (const LSClass*) const override;

	bool operator <= (const LSValue*) const override;
	bool operator <= (const LSNull*) const override;
	bool operator <= (const LSBoolean*) const override;
	bool operator <= (const LSNumber*) const override;
	bool operator <= (const LSString*) const override;
	bool operator <= (const LSArray<LSValue*>*) const override;
	bool operator <= (const LSFunction*) const override;
	bool operator <= (const LSObject*) const override;
	bool operator <= (const LSClass*) const override;

	bool operator >= (const LSValue*) const override;
	bool operator >= (const LSNull*) const override;
	bool operator >= (const LSBoolean*) const override;
	bool operator >= (const LSNumber*) const override;
	bool operator >= (const LSString*) const override;
	bool operator >= (const LSArray<LSValue*>*) const override;
	bool operator >= (const LSFunction*) const override;
	bool operator >= (const LSObject*) const override;
	bool operator >= (const LSClass*) const override;

	bool in(const LSValue*) const override;

	LSValue* at(const LSValue* value) const override;
	LSValue** atL(const LSValue* value) override;

	LSValue* range(int start, int end) const override;
	LSValue* rangeL(int start, int end) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	LSValue* abso() const override;

	std::ostream& print(std::ostream& os) const override;
	std::string json() const override;

	LSValue* clone() const override;

	LSValue* getClass() const override;

	virtual const BaseRawType* getRawType() const override;

	int typeID() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSArray.tcc"
#endif

#endif
