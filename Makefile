SRC_DIR := . src src/vm src/vm/value src/vm/standard src/doc \
src/compiler src/compiler/lexical src/compiler/syntaxic src/compiler/semantic \
src/compiler/value src/compiler/instruction src/util lib benchmark test

SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))

BUILD_DIR := $(addprefix build/default/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/shared/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/coverage/,$(SRC_DIR))

OBJ := $(patsubst %.cpp,build/default/%.o,$(SRC))
OBJ_LIB := $(patsubst %.cpp,build/shared/%.o,$(SRC))
OBJ_COVERAGE := $(patsubst %.cpp,build/coverage/%.o,$(SRC))

OPTIM := -O2
FLAGS := -std=c++14 -g3 -Wall -Wextra
LIBS := -ljit

.PHONY: test

# Main build task, default build
build/leekscript: $(BUILD_DIR) $(OBJ)
	g++ $(FLAGS) -o build/leekscript $(OBJ) $(LIBS)
	@echo "---------------"
	@echo "Build finished!"
	@echo "---------------"

fast:
	@make -j8

build/default/%.o: %.cpp
	g++ -c $(OPTIM) $(FLAGS) -o "$@" "$<"
	
build/shared/%.o: %.cpp
	g++ -c $(OPTIM) $(FLAGS) -fPIC -o "$@" "$<"
	
build/coverage/%.o: %.cpp
	g++ -c $(FLAGS) -O0 -fprofile-arcs -ftest-coverage -o "$@" "$<"

$(BUILD_DIR):
	@mkdir -p $@
	
# Build the shared library version of the leekscript
# (libleekscript.so in build/)
build/libleekscript.so: $(BUILD_DIR) $(OBJ_LIB)
	g++ $(FLAGS) -shared -o build/libleekscript.so $(OBJ_LIB) $(LIBS)
	@echo "-----------------------"
	@echo "Library build finished!"
	@echo "-----------------------"
lib: build/libleekscript.so

# Install the shared library by copying the libleekscript.so file
# into /usr/lib/ folder.
install: lib
	cp build/libleekscript.so /usr/lib/
	@find -iregex '.*\.\(hpp\|h\|tcc\)' | cpio -updm /usr/include/leekscript/
	@echo "------------------"
	@echo "Library installed!"
	@echo "------------------"
	
# Build with coverage flags enabled
build/leekscript-coverage: $(BUILD_DIR) $(OBJ_COVERAGE)
	g++ $(FLAGS) -fprofile-arcs -ftest-coverage -o build/leekscript-coverage $(OBJ_COVERAGE) $(LIBS)
	@echo "--------------------------"
	@echo "Build (coverage) finished!"
	@echo "--------------------------"
coverage: build/leekscript-coverage

# Run tests/
test: build/leekscript
	@build/leekscript -test

# Travis task, useless in local.
# Build a leekscript docker image, compile, run tests and run cpp-coveralls
# (coverage results are sent to coveralls.io).
travis:
	docker build -t leekscript .
	docker run -e COVERALLS_REPO_TOKEN="$$COVERALLS_REPO_TOKEN" leekscript /bin/bash -c "cd leekscript; make coverage && build/leekscript-coverage -test && cpp-coveralls --gcov-options='-r'"

# Coverage results with lcov.
# `apt-get install lcov`
html-coverage: coverage
	mkdir -p build/html
	cp -R src/ build/coverage/src/
	lcov --quiet --no-external --rc lcov_branch_coverage=1 --zerocounters --directory build/coverage/src --base-directory build/coverage/src
	lcov --quiet --no-external --rc lcov_branch_coverage=1 --capture --initial --directory build/coverage/src --base-directory build/coverage/src --output-file build/html/app.info
	build/leekscript-coverage -test
	lcov --quiet --no-external --rc lcov_branch_coverage=1 --no-checksum --directory build/coverage/src --base-directory build/coverage/src --capture --output-file build/html/app.info
	cd build/html; genhtml --branch-coverage app.info

# Clean every build files by destroying the build/ folder.
clean:
	rm -rf build
	@echo "----------------"
	@echo "Project cleaned."
	@echo "----------------"

# Line couning with cloc.
# `apt-get install cloc`
cloc:
	cloc . --exclude-dir=.git,lib,build
