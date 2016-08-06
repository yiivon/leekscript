#ifndef LS_MAP_BASE
#define LS_MAP_BASE

#include "../LSValue.hpp"
#include <map>

namespace ls {

template <class K>
struct lsmap_less {
	bool operator() (K lhs, K rhs) const;
};

template <class K, class T>
using LSMapIterator = typename std::map<K, T>::iterator;

template <class K, class T>
class LSMap : public LSValue, public std::map<K, T, lsmap_less<K>> {
public:
	typedef std::pair<const K,T> value_type;

	static LSValue* map_class;

	LSMap();
	virtual ~LSMap();

	/*
	 * Map methods;
	 */
	LSMap<K,T>* ls_insert(const value_type& x);
	LSMap<K,T>* ls_clear();
	LSMap<K,T>* ls_erase(K key);

	/*
	 * LSValue methods;
	 */
	bool isTrue() const override;

	LSValue* operator + (const LSValue*) const override;
	LSValue* operator += (LSValue*) override;
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
	//bool operator == (const LSMap<LSValue*>*) const override;

	virtual bool operator < (const LSValue*) const override;
	//virtual bool operator < (const LSMap*) const override;

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
#include "LSMap.tcc"
#endif

#endif
