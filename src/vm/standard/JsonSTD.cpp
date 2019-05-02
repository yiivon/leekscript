#include "JsonSTD.hpp"
#include "../LSValue.hpp"
#include "../value/LSNull.hpp"
#include "../value/LSString.hpp"
#include "../value/LSNumber.hpp"
#include <chrono>

namespace ls {

JsonSTD::JsonSTD() : Module("Json") {

	method("encode", {
		{Type::string(), {Type::const_any()}, (void*) &JsonSTD::encode}
	});
	method("decode", {
		{Type::any(), {Type::const_string()}, (void*) &JsonSTD::decode},
	});
}

Compiler::value JsonSTD::encode(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t == Type::integer()) {
		return c.insn_call(Type::string(), args, +[](int v) {
			return new LSString(std::to_string(v));
		});
	} else if (args[0].t == Type::long_()) {
		return c.insn_call(Type::string(), args, +[](long v) {
			return new LSString(std::to_string(v));
		});
	} else if (args[0].t == Type::real()) {
		return c.insn_call(Type::string(), args, +[](double v) {
			return new LSString(LSNumber::print(v));
		});
	} else if (args[0].t == Type::boolean()) {
		return c.insn_call(Type::string(), args, +[](bool b) {
			return new LSString(b ? "true" : "false");
		});
	} else if (args[0].t.is_mpz_ptr()) {
		auto s = c.insn_call(Type::string(), args, +[](__mpz_struct* v) {
			char buff[10000];
			mpz_get_str(buff, 10, v);
			return new LSString(buff);
		});
		if (args[0].t.temporary) {
			c.insn_delete_mpz(args[0]);
		}
		return s;
	}
	// Default type : pointer
	return c.insn_call(Type::string(), args, (void*) &LSValue::ls_json);
}

Compiler::value JsonSTD::decode(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::any(), args, (void*) +[](LSString* string) {
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
