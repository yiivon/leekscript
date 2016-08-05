#ifndef LS_MAP_BASE
#define LS_MAP_BASE

#include "../LSValue.hpp"
#include <map>

namespace ls {

struct lsvalue_less {
	bool operator() (const LSValue* lhs, const LSValue* rhs) const {
		return rhs->operator < (lhs);
	}
};

template <typename T>
using LSMapIterator = typename std::map<LSValue*, T>::iterator;

template <typename T>
class LSMap : public LSValue, public std::map<LSValue*, T, lsvalue_less> {
public:
	LSMap();
	LSMap(std::initializer_list<std::pair<LSValue*, T>>);
	virtual ~LSMap();

	/*
	 * Map methods
	 */

	/*
	 * LSValue methods;
	 */
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

};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSMap.tcc"
#endif

#endif
