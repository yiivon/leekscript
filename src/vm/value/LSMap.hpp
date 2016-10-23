#ifndef LS_MAP_BASE
#define LS_MAP_BASE

#include "../LSValue.hpp"
#include <map>

namespace ls {

template <typename K>
struct lsmap_less {
	bool operator() (K lhs, K rhs) const;
};

template <typename K = LSValue*, typename T = LSValue*>
class LSMap : public LSValue, public std::map<K, T, lsmap_less<K>> {
public:
	static LSValue* map_class;

	LSMap();
	virtual ~LSMap();

	/*
	 * Map methods;
	 */
	bool ls_insert(K key, T value);
	LSMap<K, T>* ls_clear();
	bool ls_erase(K key);
	T ls_look(K key, T def);

	/*
	 * LSValue methods;
	 */
	virtual bool isTrue() const override;

	LSVALUE_OPERATORS

	template <typename K2, typename T2>
	bool eq(const LSMap<K2, T2>*) const;
	/*
	virtual bool eq(const LSMap<LSValue*,int>*) const override;
	virtual bool eq(const LSMap<LSValue*,double>*) const override;
	virtual bool eq(const LSMap<int,LSValue*>*) const override;
	virtual bool eq(const LSMap<int,int>*) const override;
	virtual bool eq(const LSMap<int,double>*) const override;
	*/

	template <typename K2, typename T2>
	bool lt(const LSMap<K2, T2>*) const;
	/*
	virtual bool lt(const LSMap<LSValue*,LSValue*>*) const override;
	virtual bool lt(const LSMap<LSValue*,int>*) const override;
	virtual bool lt(const LSMap<LSValue*,double>*) const override;
	virtual bool lt(const LSMap<int,LSValue*>*) const override;
	virtual bool lt(const LSMap<int,int>*) const override;
	virtual bool lt(const LSMap<int,double>*) const override;
	*/

	virtual LSValue* at(const LSValue* key) const override;
	LSValue* at_real_ptr(double) const;
	LSValue* at_int_ptr(int) const;
	int at_ptr_int(const LSValue* key) const;
	int at_int_int(int key) const;
	int at_real_int(double key) const;
	double at_int_real(int) const;
	double at_real_real(double) const;
	double at_ptr_real(const LSValue* key) const;

	virtual LSValue** atL(const LSValue* key) override;
	int* atLv(const LSValue* key) const;

	virtual bool in(LSValue*) const override;

	virtual std::ostream& print(std::ostream&) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual LSValue* getClass() const override;
	virtual int typeID() const override { return 6; }
	virtual const BaseRawType* getRawType() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSMap.tcc"
#endif

#endif
