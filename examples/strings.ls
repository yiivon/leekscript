let s = "1010111010" × 20
let l = s.size()
print("size : " + l)

let r1 = ''
let r2 = ''

for var i = 0; i < 200; i += 1 {
	r1 += s[i]
	r2 = s[i] + r2
}

print(r2)

let m = ['A', 'T', 'C', 'G']
var c = 0
var tests = 10000
let adn = ''

for var k = 0; k < tests; k++ {

	adn = ''
	
	for var j = 0; j < 200; j++ {
		adn += m[Number.randInt(0, 4)]
	}
	if adn.contains('GAGA') { c++ }
}
print(adn)

100 * ((c + 0.000001) / tests) + '%' // ~ 52%