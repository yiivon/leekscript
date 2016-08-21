/*
 * Describe an array in LeekScript
 */
#ifndef LS_ARRAY_BASE
#define LS_ARRAY_BASE

#include <vector>
#include <iterator>

#include "../LSValue.hpp"

namespace ls {

template <typename T>
class LSArray : public LSValue, public std::vector<T> {
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
	void push_clone(T value); // clone increment and push
	void push_move(T value); // clone (if not temporaray) increment and push
	void push_inc(T value); // increment (if not native) and push

	LSArray<T>* ls_clear();
	T ls_remove(int index);
	bool ls_remove_element(T element);
	bool ls_empty();
	LSValue* ls_pop();
	int ls_size();
	T ls_sum();
	double ls_average();
	LSValue* ls_first();
	LSValue* ls_last();
	int atv(const int i);
	int* atLv(int i);
	LSArray<LSValue*>* ls_map(const void*);
	LSArray<int>* ls_map_int(const void*);
	LSArray<double>* ls_map_real(const void*);
	LSArray<LSValue*>* ls_chunk(int size = 1);
	LSValue* ls_unique();
	LSValue* ls_sort();
	void ls_iter(const void* fun);
	bool ls_contains(T val);
	LSValue* ls_push(T val);
	LSArray<T>* ls_push_all_ptr(LSArray<LSValue*>* array);
	LSArray<T>* ls_push_all_int(LSArray<int>* array);
	LSArray<T>* ls_push_all_flo(LSArray<double>* array);
	LSArray<T>* ls_shuffle();
	LSArray<T>* ls_reverse();
	LSArray<T>* ls_filter(const void* fun);
	LSValue* ls_foldLeft(const void* fun, LSValue* initial);
	LSValue* ls_foldRight(const void* fun, LSValue* initial);
	LSArray<T>* ls_insert(T value, int pos);
	LSArray<LSValue*>* ls_partition(const void* fun);
	LSArray<LSValue*>* ls_map2(LSArray<LSValue*>*, const void* fun);
	LSArray<LSValue*>* ls_map2_int(LSArray<int>*, const void* fun);
	int ls_search(T needle, int start);
	LSString* ls_join(LSString* glue);
	LSArray<T>* ls_fill(T element, int size);
	T ls_max();
	T ls_min();

	/*
	 * LSValue methods
	 */
	bool isTrue() const override;

	LSValue* operator ! () const override;
	LSValue* operator ~ () const override;

	LSValue* ls_radd(LSValue*) override;
	LSValue* ls_add(LSNull*) override;
	LSValue* ls_add(LSBoolean*) override;
	LSValue* ls_add(LSNumber*) override;
	LSValue* ls_add(LSString*) override;
	LSValue* ls_add(LSArray<LSValue*>*) override;
	LSValue* ls_add(LSArray<int>*) override;
	LSValue* ls_add(LSArray<double>*) override;
	LSValue* ls_add(LSObject*) override;
	LSValue* ls_add(LSFunction*) override;
	LSValue* ls_add(LSClass*) override;

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
	LSValue* operator -= (LSValue*) override;
	LSValue* operator * (const LSValue*) const override;
	LSValue* operator *= (LSValue*) override;
	LSValue* operator / (const LSValue*) const override;
	LSValue* operator /= (LSValue*) override;
	LSValue* poww(const LSValue*) const override;
	LSValue* pow_eq(LSValue*) override;
	LSValue* operator % (const LSValue*) const override;
	LSValue* operator %= (LSValue*) override;


	bool operator == (const LSValue*) const override;
	bool operator == (const LSArray<LSValue*>*) const override;

	bool operator < (const LSValue*) const override;
	bool operator < (const LSNull*) const override;
	bool operator < (const LSBoolean*) const override;
	bool operator < (const LSNumber*) const override;
	bool operator < (const LSString*) const override;
	bool operator < (const LSArray<LSValue*>*) const override;
	bool operator < (const LSArray<int>*) const override;
	bool operator < (const LSArray<double>*) const override;
	bool operator < (const LSFunction*) const override;
	bool operator < (const LSObject*) const override;
	bool operator < (const LSClass*) const override;


	bool in(LSValue*) const override;

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
