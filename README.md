LeekScript language
===================

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
The executable "leekscript" is in the build/ folder.

Usage
-----

Run a LeekScript top-level
```
./LeekScript
```

Run the tests
```
./LeekScript -test
```

Execute a file
```
./LeekScript -f my_file.ls
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