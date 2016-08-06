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

LSMap<LSValue*,LSValue*>* map_insert_ptr_ptr(LSMap<LSValue*,LSValue*>* map, LSValue* key, LSValue* value);
LSMap<LSValue*,int>* map_insert_ptr_int(LSMap<LSValue*,int>* map, LSValue* key, int value);
LSMap<LSValue*,double>* map_insert_ptr_float(LSMap<LSValue*,double>* map, LSValue* key, double value);
LSMap<int,LSValue*>* map_insert_int_ptr(LSMap<int,LSValue*>* map, int key, LSValue* value);
LSMap<int,int>* map_insert_int_int(LSMap<int,int>* map, int key, int value);
LSMap<int,double>* map_insert_int_float(LSMap<int,double>* map, int key, double value);

LSMap<LSValue*,LSValue*>* map_clear_ptr_ptr(LSMap<LSValue*,LSValue*>* map);
LSMap<LSValue*,int>* map_clear_ptr_int(LSMap<LSValue*,int>* map);
LSMap<LSValue*,double>* map_clear_ptr_float(LSMap<LSValue*,double>* map);
LSMap<int,LSValue*>* map_clear_int_ptr(LSMap<int,LSValue*>* map);
LSMap<int,int>* map_clear_int_int(LSMap<int,int>* map);
LSMap<int,double>* map_clear_int_float(LSMap<int,double>* map);

}

#endif
