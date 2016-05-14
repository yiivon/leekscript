#ifndef LS_MAP_BASE
#define LS_MAP_BASE

#include "LSArray.hpp"

struct lsvalue_less {
	bool operator() (const LSValue* lhs, const LSValue* rhs) const {
		return rhs->operator < (lhs);
	}
};

template <typename T>
using LSMapIterator = typename std::map<LSValue*, T>::iterator;

template <typename T>
class LSMap : public LSArray<T> {
public:

	int index = 0;
	std::map<LSValue*, T, lsvalue_less> values;

	LSMap();
	LSMap(std::initializer_list<std::pair<LSValue*, T>>);
	virtual ~LSMap();

	/*
	 * Map methods
	 */
	LSValue* pop();
	void push_clone(const T value);
	void push_no_clone(T value);
	void push_key_clone(LSValue* key, const T var);
	void push_key_no_clone(LSValue* key, T var);
	LSValue* remove_key(LSValue* key);
	virtual void clear();
	size_t size() const override;
	double sum() const override;
	double average() const override;

	/*
	 * LSValue methods;
	 */
	virtual bool isTrue() const override;
	virtual LSValue* operator ! () const override;

	LSValue* operator + (const LSNull* nulll) const;
	LSValue* operator + (const LSBoolean* boolean) const;
	LSValue* operator + (const LSNumber* number) const;
	LSValue* operator + (const LSString* string) const;
	LSValue* operator + (const LSArray<LSValue*>* array) const;
	LSValue* operator + (const LSMap<LSValue*>* map) const;

	LSValue* operator += (const LSMap<LSValue*>* array);

	bool operator == (const LSMap<LSValue*>* v) const;

	virtual bool in(const LSValue* key) const override;
	virtual LSValue* at(const LSValue* key) const override;
	virtual LSValue** atL(const LSValue* key) override;

	LSValue* range(int, int end) const override;

	LSValue* attr(const LSValue* key) const override;
	LSValue* abso() const override;

	LSValue* clone() const override;
	std::ostream& print(std::ostream& os) const override;

	std::string json() const override;
};

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSMap.tcc"
#endif

#endif
