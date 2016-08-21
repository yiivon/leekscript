#ifndef LS_SET_BASE
#define LS_SET_BASE

#include "../LSValue.hpp"
#include <set>

namespace ls {

template <typename K>
struct lsset_less {
	bool operator() (K lhs, K rhs) const;
};

template <typename T>
class LSSet : public LSValue, public std::set<T, lsset_less<T>> {
public:
	static LSValue* set_class;

	LSSet();
	LSSet(const LSSet<T>& other);
	virtual ~LSSet();

	/*
	 * LSSet methods;
	 */
	bool ls_insert(T value);
	LSSet<T>* ls_clear();
	bool ls_erase(T value);
	bool ls_contains(T value);

	/*
	 * LSValue methods;
	 */
	virtual bool isTrue() const override;

	virtual LSValue* ls_radd(LSValue* value) override { return value->ls_add(this); }
	virtual LSValue* operator += (LSValue*) override;
	virtual LSValue* operator - (const LSValue*) const override;
	virtual LSValue* operator -= (LSValue*) override;
	virtual LSValue* operator * (const LSValue*) const override;
	virtual LSValue* operator *= (LSValue*) override;
	virtual LSValue* operator / (const LSValue*) const override;
	virtual LSValue* operator /= (LSValue*) override;
	virtual LSValue* poww(const LSValue*) const override;
	virtual LSValue* pow_eq(LSValue*) override;
	virtual LSValue* operator % (const LSValue*) const override;
	virtual LSValue* operator %= (LSValue*) override;
	virtual bool operator == (const LSValue*) const override;
	virtual bool operator == (const LSSet<LSValue*>*) const override;
	virtual bool operator == (const LSSet<int>*) const override;
	virtual bool operator == (const LSSet<double>*) const override;

	virtual bool operator < (const LSValue*) const override;
//	virtual bool operator < (const LSSet<LSValue*>*) const override;

	bool in(LSValue*) const override;
	virtual LSValue* at(const LSValue* key) const override;
	virtual LSValue** atL(const LSValue* key) override;
	virtual std::ostream& print(std::ostream&) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual LSValue* getClass() const override;
	virtual int typeID() const override;
	virtual const BaseRawType* getRawType() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSSet.tcc"
#endif

#endif
