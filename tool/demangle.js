'use strict'
let fs = require("fs")
let spawn = require('child_process').spawnSync
let escape = require('escape-html')

let file = process.argv[2]
let data = fs.readFileSync(file).toString()
let result = data.replace(/(<td class="coverFn"><a href=".*">)(.*?)(<\/a><\/td>)/g, function(a, b, c, d) {
	return b + escape(spawn('c++filt', [c]).stdout.toString()) + d
})
console.log(result)
