var l = 0.1
var ll = 0.1
var e = 1.00001
var i

for (i = 0; i < 2000; ++i) {
	if (e == l) {
		System.print("Epsilon found: ")
		System.print(ll)
		break
	}
	ll = l
	l = e
	e *= 0.5
}

"after " + i + " steps"