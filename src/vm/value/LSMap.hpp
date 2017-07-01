#ifndef LS_MAP_BASE
#define LS_MAP_BASE

#include "../LSValue.hpp"
#include <map>

namespace ls {

template <typename K>
struct lsmap_less {
	bool operator() (K lhs, K rhs) const;
};

template <typename K, typename V>
class LSMap : public LSValue, public std::map<K, V, lsmap_less<K>> {
public:
	static LSValue* clazz;

	LSMap();
	virtual ~LSMap();

	/*
	 * Map methods;
	 */
	bool ls_insert(K key, V value);
	LSMap<K, V>* ls_clear();
	bool ls_erase(K key);
	V ls_look(K key, V def);
	V ls_min();
	K ls_minKey();
	LSArray<V>* values() const;
	template <class K2, class V2>
	bool map_equals(const LSMap<K2, V2>* map) const;
	template <class K2, class V2>
	bool map_lt(const LSMap<K2, V2>* map) const;
	void ls_iter(LSFunction<LSValue*>* function) const;

	/*
	 * LSValue methods;
	 */
	virtual bool to_bool() const override;
	virtual bool ls_not() const override;

	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;

	V at(const K key) const;

	virtual LSValue** atL(const LSValue* key) override;
	V* atL_base(K key) const;

	bool in(K) const;

	virtual std::ostream& dump(std::ostream&, int level) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual LSValue* getClass() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSMap.tcc"
#endif

#endif
