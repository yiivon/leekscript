#include "JsonSTD.hpp"

#include "../LSValue.hpp"
#include "../value/LSNull.hpp"
#include "../value/LSString.hpp"
#include "../value/LSNumber.hpp"
#include <chrono>

namespace ls {

JsonSTD::JsonSTD() : Module("Json") {

	static_method("encode", {
		{Type::STRING, {Type::UNKNOWN}, (void*) &JsonSTD::encode}
	});
	static_method("decode", {
		{Type::UNKNOWN, {Type::STRING}, (void*) &JsonSTD::decode}
	});
}

Compiler::value JsonSTD::encode(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t == Type::INTEGER) {
		return c.insn_call(Type::STRING, args, +[](int v) {
			return new LSString(std::to_string(v));
		});
	}
	if (args[0].t == Type::LONG) {
		return c.insn_call(Type::STRING, args, +[](long v) {
			return new LSString(std::to_string(v));
		});
	}
	if (args[0].t == Type::REAL) {
		return c.insn_call(Type::STRING, args, +[](double v) {
			return new LSString(LSNumber::print(v));
		});
	}
	if (args[0].t == Type::BOOLEAN) {
		return c.insn_call(Type::STRING, args, +[](bool b) {
			return new LSString(b ? "true" : "false");
		});
	}
	if (args[0].t.nature == Nature::POINTER) {
		return c.insn_call(Type::STRING, args, (void*) &LSValue::ls_json);
	}
	std::cout << "Type non supporté !" << std::endl;
	throw new std::exception();
	return {nullptr, Type::VOID};
}

Compiler::value JsonSTD::decode(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::POINTER, args, (void*) +[](LSString* string) {
		try {
			Json json = Json::parse(*string);
			LSValue::delete_temporary(string);
			return LSValue::get_from_json(json);
		} catch (...) {
			LSValue::delete_temporary(string);
			return LSNull::get();
		}
	});
}

}
