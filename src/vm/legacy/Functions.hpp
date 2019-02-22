#include <string>
#include <vector>
#include "../../compiler/semantic/Callable.hpp"

namespace ls {

class VM;
class LSValue;
class LSFunction;
template <class T> class LSArray;
class LSString;
class Type;

namespace legacy {

class Functions {
public:

	// Include desired functions into a vm
	static void create(VM* vm);
	static void add(VM* vm, std::string name, Callable);

	/*
	 * V1 functions
	 */
	// Strings
	static LSValue* v1_charAt(LSString* v, int p);
	static LSValue* v1_replace(LSString* string, LSString* from, LSString* to);
	// Arrays
	static int v1_count(LSArray<LSValue*>* v);
	static bool v1_inArray(LSArray<LSValue*>* a, LSValue* b);
	static LSValue* v1_push(LSArray<LSValue*>* a, LSValue* b);
	static LSValue* v1_pushAll(LSArray<LSValue*>* a, LSArray<LSValue*>* b);
	static void v1_fill(LSArray<LSValue*>* a, LSValue* b);
	static void v1_fill_2(LSArray<LSValue*>* a, LSValue* b, int c);
	// Utils
	static void v1_debug(LSValue* v);
};

}
}
