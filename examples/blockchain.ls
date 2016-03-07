var f = function(block) {

	print(block)

	if (block.par == 'abc') {
		return false;
	}
	return block.par
}

var b1 = {par: 'abc'}
print(b1.par)

print(f(b1))