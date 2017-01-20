#define NUMBER_TYPE double

#ifndef LSNUMBER_H_
#define LSNUMBER_H_

#include <iostream>
#include <gmp.h>
#include <gmpxx.h>

#include "../LSValue.hpp"
#include "LSString.hpp"
#include "LSClass.hpp"
#include "../../../lib/json.hpp"
#include "../Type.hpp"

namespace ls {

class LSNumber : public LSValue {
public:

	NUMBER_TYPE value;

	static LSClass* number_class;
	static LSNumber* get(NUMBER_TYPE);
	static std::string print(double);

	LSNumber();
	LSNumber(NUMBER_TYPE value);
	LSNumber(Json& data);

	virtual ~LSNumber();

	/*
	 * LSNumber
	 */
	LSValue* ls_fold(LSFunction<LSValue*>* function, LSValue* v0);

	/*
	 * LSValue
	 */
	bool isTrue() const override;

	LSValue* ls_minus() override;
	LSValue* ls_not() override;
	LSValue* ls_tilde() override;
	LSValue* ls_preinc() override;
	LSValue* ls_inc() override;
	LSValue* ls_predec() override;
	LSValue* ls_dec() override;

	virtual LSValue* add(LSValue*) override;
	virtual LSValue* add_eq(LSValue*) override;
	virtual LSValue* sub(LSValue*) override;
	virtual LSValue* sub_eq(LSValue*) override;
	virtual LSValue* mul(LSValue*) override;
	virtual LSValue* div(LSValue*) override;
	LSVALUE_OPERATORS

	LSValue* ls_mul_eq(LSNull*) override;
	LSValue* ls_mul_eq(LSBoolean*) override;
	LSValue* ls_mul_eq(LSNumber*) override;

	LSValue* ls_int_div(LSNumber*) override;

	LSValue* ls_div_eq(LSNull*) override;
	LSValue* ls_div_eq(LSBoolean*) override;
	LSValue* ls_div_eq(LSNumber*) override;

	LSValue* ls_pow(LSNull*) override;
	LSValue* ls_pow(LSBoolean*) override;
	LSValue* ls_pow(LSNumber*) override;

	LSValue* ls_pow_eq(LSNull*) override;
	LSValue* ls_pow_eq(LSBoolean*) override;
	LSValue* ls_pow_eq(LSNumber*) override;

	LSValue* ls_mod(LSNull*) override;
	LSValue* ls_mod(LSBoolean*) override;
	LSValue* ls_mod(LSNumber*) override;

	LSValue* ls_mod_eq(LSNull*) override;
	LSValue* ls_mod_eq(LSBoolean*) override;
	LSValue* ls_mod_eq(LSNumber*) override;

	bool operator == (int value) const override;
	bool operator == (double value) const override;

	bool eq(const LSNumber*) const override;
	bool lt(const LSNumber*) const override;

	LSValue* at(const LSValue* value) const override;
	LSValue** atL(const LSValue* value) override;

	LSValue* abso() const override;

	int integer() const override;
	double real() const override;

	LSValue* clone() const override;

	std::ostream& dump(std::ostream& os) const override;
	std::string json() const override;
	std::string toString() const;

	LSValue* getClass() const override;

	bool isInteger() const;

	int typeID() const override;
};

}

#endif
