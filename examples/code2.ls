let values = [1, 2, 33243, 234, 3247, 234, 123, 865, 45342, 2, 654, 67541, 4353, 7657]
let even = x -> x % 2 == 0
let somme = 0

for v in values {
	//__debug(v + " : " + even(v))
	somme += v
}

print("Somme : " + somme)
print("Moyenne : " + (somme / values.size))


for i = 1; i < 11; ++i do
	for j = 1; j < 11; ++j do
		print(j + ' × ' + i + " = " + i * j)
	end
	print("--------")
end

for k : e in values do
	values[k] = e / somme
end

print(values)
print(| -5 |)

let b = 10
let a = 5
++b

let f = x -> y -> x + y
let g = f(5)
g(12)
