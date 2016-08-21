let f = function(number) {
	for (let k = 1; 36 * k * k - 12 * k < number; k++) {
		if ((number % (6 * k + 1) == 0) or (number % (6 * k - 1) == 0)) {
			return false
		}
	}
	return true
}

let c = 2

for let i = 5; i < 1000000; i += 6 do
	if f(i) c++
	if f(i + 2) c++
end

c
