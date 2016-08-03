let a = 2

let f = x -> x + a

f(9)

let g = x -> f(a, x)

let fact = n -> n * fact(n - 1)

let deep = function() {
	let o = 12
	let f = x -> x ** a + o
}