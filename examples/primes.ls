let f = function(number) {

	if ((number % 2 == 0) or (number < 2) or (number % 3 == 0 and number != 3)) {
		return false;
	}

	for (let k = 1; 36 * k * k - 12 * k < number; k++) {
		if ((number % (6 * k + 1) == 0) or (number % (6 * k - 1) == 0)) {
			return false;
		}
	}
	return true;
}

let c = 1

for (let i = 3; i < 1000000; i += 2) do
	if (f(i)) {
		c++
	}
end

return c

/*

new Number(12)   ==>   new Number // params {12}

[Number][0](12)

var digits = [1]

for p = 1; p <= 100; p++ {
	for j = 0; j < digits.size(); ++j {
		digits[j] *= p
	}
	for i = 0; i < digits.size(); ++i {
		if (9 < digits[i]) {
			let m = digits[i] % 10
			var q = (digits[i] - m) / 10
			digits[i] = m
			if (i < digits.size() - 1) {
				digits[i + 1] += q
			} else {
				digits += q
				break
			}
		}
	}
	var n = digits.size() - 1
	while (9 < digits[n]) {
		let mo = digits[n] % 10
		let qu = (digits[n] - mo) / 10
		digits[n] = mo
		digits += qu
		n++
	}
}
print(~digits)
digits.sum()