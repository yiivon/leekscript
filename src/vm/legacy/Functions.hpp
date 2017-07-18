#include <string>
#include <vector>

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
	static void add(VM* vm, std::string name, Type return_type, std::vector<Type> args, void* fun);

	/*
	 * V1 functions
	 */
	// Strings
	static LSValue* v1_charAt(LSString* v, int p);
	static LSValue* v1_replace(LSString* string, LSString* from, LSString* to);
	// Arrays
	static int v1_count(LSArray<LSValue*>* v);
	static LSValue* v1_pushAll(LSArray<LSValue*>* a, LSArray<LSValue*>* b);
	// Utils
	static void v1_debug(LSValue* v);
};

}
}
