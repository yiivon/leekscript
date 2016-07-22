function is_perm(a, b) {
    var sa = String.split('' + a, '')
    var sb = String.split('' + b, '')
    if (sa.size() != sb.size()) { return false }
    for var i in sa {
        var p = sb.search(i, 0)
        if (p == -1) { return false }
        Array.remove(sb, p)
    }
    return true
}

var groups = [['0']]
Array.remove(groups, 0)

var i = 5000;
while (i++) {

    var c = '' + ([i, ''][0] ** 3)

    var added = false

    for (var g in groups) {

        if (is_perm(String.number(g[0]), String.number(c)) == true) {

            Array.push(g, c)
            added = true
            
            if (g.size() >= 5) {
            	return g
            }
        }
    }
    if (added == false) {
        Array.push(groups, [c])
    }
}