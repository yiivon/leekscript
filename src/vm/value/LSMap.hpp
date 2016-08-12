#ifndef LS_MAP_BASE
#define LS_MAP_BASE

#include "../LSValue.hpp"
#include <map>

namespace ls {

template <typename K>
struct lsmap_less {
	bool operator() (K lhs, K rhs) const;
};

template <typename K, typename T>
using LSMapIterator = typename std::map<K, T>::iterator;

template <typename K, typename T>
class LSMap : public LSValue, public std::map<K, T, lsmap_less<K>> {
public:
	static LSValue* map_class;

	LSMap();
	virtual ~LSMap();

	/*
	 * Map methods;
	 */
	bool ls_insert(K key, T value);
	LSMap<K,T>* ls_clear();
	bool ls_erase(K key);
	T ls_look(K key, T def);

	/*
	 * LSValue methods;
	 */
	virtual bool isTrue() const override;

	virtual LSValue* operator + (const LSValue*) const override;
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
	virtual bool operator == (const LSMap<LSValue*,LSValue*>*) const override;
	virtual bool operator == (const LSMap<LSValue*,int>*) const override;
	virtual bool operator == (const LSMap<LSValue*,double>*) const override;
	virtual bool operator == (const LSMap<int,LSValue*>*) const override;
	virtual bool operator == (const LSMap<int,int>*) const override;
	virtual bool operator == (const LSMap<int,double>*) const override;

	virtual bool operator < (const LSValue*) const override;
//	virtual bool operator < (const LSMap<LSValue*,LSValue*>*) const override;

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
