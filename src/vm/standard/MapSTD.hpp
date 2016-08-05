#ifndef VM_STANDARD_MAPSTD_HPP_
#define VM_STANDARD_MAPSTD_HPP_

#include "../value/LSFunction.hpp"
#include "../Module.hpp"

namespace ls {

class MapSTD : public Module {
public:
	MapSTD();
};

int map_size(const LSMap<LSValue*,LSValue*>* map);

}

#endif
