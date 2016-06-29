#ifndef VM_STANDARD_STRINGSTD_HPP_
#define VM_STANDARD_STRINGSTD_HPP_

#include "../VM.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class LSString;
class LSNumber;

class StringSTD : public Module {
public:
	StringSTD();
	virtual ~StringSTD();
};

LSValue* string_charAt(LSString* string, LSNumber* index);
LSValue* string_contains(LSString* haystack, LSString* needle);
LSValue* string_endsWith(LSString* string, LSString* ending);
int string_indexOf(LSString* haystack, LSString* needle);
LSValue* string_length(LSString* string);
LSValue* string_map(const LSString* string, void* fun);
LSValue* string_replace(LSString* string, LSString* from, LSString* to);
LSValue* string_reverse(LSString* string);
LSValue* string_size(LSString* string);
LSValue* string_split(LSString* string, LSString* delimiter);
LSValue* string_startsWith(const LSString* string, const LSString* starting);
LSValue* string_substring(LSString* string, LSNumber* start, LSNumber* length);
LSValue* string_toLower(LSString* string);
LSValue* string_toUpper(LSString* string);
LSValue* string_toArray(const LSString* string);

}

#endif
