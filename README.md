LeekScript language
===================

[![Build Status](https://travis-ci.org/leek-wars/leekscript.svg?branch=master)](https://travis-ci.org/leek-wars/leekscript)
[![GPL Licence](https://badges.frapsoft.com/os/gpl/gpl.svg?v=103)](https://opensource.org/licenses/GPL-3.0/)

LeekScript is a language initially designed for Leek Wars.
The project is still in development, and shouldn't be used for the moment.


Demo
----

https://leekscript.com

Building
--------

```
make
```
The executable `leekscript` is in the `build/` folder.

Export as shared library
```
make lib
```
`libleekscript.so` will be in `build/` folder. You can `make install` to copy it in `/usr/lib/`.

Usage
-----

Run a LeekScript top-level
```
./leekscript
```

Run the tests
```
./leekscript -test
```

Execute a file
```
./leekscript -f my_file.ls
```

Run a code, and get the result as JSON
```
./leekscript -e "my code" "{}"
```


Libraries used
--------------
* libjit, by Rhys Weatherley, under GPL license
* gason, by Ivan Vashchaev, under MIT license

License
-------

Distributed under the GPL3 license. Copyright (c) 2016, Pierre Lauprêtre
