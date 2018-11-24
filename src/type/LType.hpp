#ifndef L_TYPE_HPP
#define L_TYPE_HPP

#include <memory>
#include <string>
#include <vector>
#include "Base_type.hpp"

namespace ls {

class LType : public Base_type {
private:
	std::vector<std::shared_ptr<Base_type>> _types; 
public:
	LType();
	std::vector<std::shared_ptr<Base_type>> types() const;
	void add(const std::shared_ptr<Base_type> type);

	virtual bool compatible(std::shared_ptr<Base_type>) const override;
	virtual bool all(std::function<bool(const Base_type* const)>) const override;
	virtual const std::string clazz() const { return "?"; }
	virtual const std::string json_name() const { return "?"; }
	virtual bool iterable() const override;
	virtual bool container() const override;
	virtual std::ostream& print(std::ostream&) const override;
};

}

#endif