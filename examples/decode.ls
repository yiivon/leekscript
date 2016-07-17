"hello".map(x -> if x == ' ' then ' ' else x.code() - 'a'.code() + 1 + ' ' end)


/*
let m = "dans leek wars creez le plus puissant poireau au monde et detruisez vos ennemis"

let c = m.map(x -> if x == ' ' then ' ' else (x.code() - 'a'.code() + 1) + "." end)

let d = m.split(' ').map(w -> w + '.')