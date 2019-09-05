{
    "targets": [
        {
            "target_name": "leekscript",
            "type": "executable",
            "sources": [
            	'<!@(find src -name "*.cpp")',
				'lib/utf8.cpp'
            ],
			"libraries": [
				"-lm", "-lgmp", "-L/usr/lib/llvm-6.0/lib", "-lLLVM-6.0"
			],
			"cflags": [
				'-O0', '-std=c++17', '-g3', '-Wall', '-Wno-pmf-conversions'
			]
        }, {
            "target_name": "leekscript-test",
            "type": "executable",
            "sources": [
            	'<!@(find src -name "*.cpp" ! -name "TopLevel.cpp")',
				'<!@(find test -name "*.cpp")',
				'lib/utf8.cpp'
            ],
			"libraries": [
				"-lm", "-lgmp", "-L/usr/lib/llvm-6.0/lib", "-lLLVM-6.0"
			],
			"cflags": [
				'-O0', '-std=c++17', '-g3', '-Wall', '-Wno-pmf-conversions'
			]
        }
    ]
}