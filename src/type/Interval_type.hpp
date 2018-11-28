#ifndef INTERVAL_TYPE_HPP
#define INTERVAL_TYPE_HPP

#include "List_type.hpp"

namespace ls {

class Interval_type : public List_type {
	static int _id;
public:
	Interval_type(int id = 1, const std::string name = "interval");
	virtual ~Interval_type();
	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif