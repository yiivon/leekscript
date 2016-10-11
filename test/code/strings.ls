let m = ['A', 'T', 'C', 'G']
var count = 0
var tests = 10000

for var k = 0; k < tests; k++ {

	let adn = ''
	for var j = 0; j < 200; j++ {
		adn += m[Number.randInt(0, 4)]
	}
	var contains = adn.contains('GAGA');
	if contains count++
}

100 * ((count + 0.000001) / tests) // ~ 52%
