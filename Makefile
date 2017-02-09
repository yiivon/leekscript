SRC_DIR := src/vm src/vm/value src/vm/standard src/doc \
src/compiler src/compiler/lexical src/compiler/syntaxic src/compiler/semantic \
src/compiler/value src/compiler/instruction src/util lib
TEST_DIR := test

SRC := $(foreach d,$(SRC_DIR),$(wildcard $(d)/*.cpp))
TEST_SRC := $(foreach d,$(TEST_DIR),$(wildcard $(d)/*.cpp))

BUILD_DIR := $(addprefix build/default/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/default/,$(TEST_DIR))
BUILD_DIR += $(addprefix build/shared/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/coverage/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/profile/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/deps/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/deps/,$(TEST_DIR))

OBJ := $(patsubst %.cpp,build/default/%.o,$(SRC))
DEPS := $(patsubst %.cpp,build/deps/%.d,$(SRC))

OBJ_TOPLEVEL = build/default/src/TopLevel.o
OBJ_BENCHMARK = build/benchmark/Benchmark.o
OBJ_TEST := $(patsubst %.cpp,build/default/%.o,$(TEST_SRC))
OBJ_LIB := $(patsubst %.cpp,build/shared/%.o,$(SRC))
OBJ_COVERAGE := $(patsubst %.cpp,build/coverage/%.o,$(SRC))
OBJ_PROFILE := $(patsubst %.cpp,build/profile/%.o,$(SRC))

OPTIM := -O2
FLAGS := -std=c++17 -g3 -Wall -Wextra -Wno-pmf-conversions
LIBS := -ljit -lgmpxx -lgmp
MAKEFLAGS += --jobs=$(shell nproc)

.PHONY: test

all: build/leekscript

# Main build task, default build
build/leekscript: $(BUILD_DIR) $(OBJ) $(OBJ_TOPLEVEL)
	g++ $(FLAGS) -o build/leekscript $(OBJ) $(OBJ_TOPLEVEL) $(LIBS)
	@echo "---------------"
	@echo "Build finished!"
	@echo "---------------"

build/default/%.o: %.cpp
	g++ -c $(OPTIM) $(DEPFLAGS) $(FLAGS) -o "$@" "$<"
	@g++ $(FLAGS) -MM -MT $@ $*.cpp -MF build/deps/$*.d

build/shared/%.o: %.cpp
	g++ -c $(OPTIM) $(FLAGS) -fPIC -o "$@" "$<"
	@g++ $(FLAGS) -MM -MT $@ $*.cpp -MF build/deps/$*.d

build/coverage/%.o: %.cpp
	g++ -c $(FLAGS) -O0 -fprofile-arcs -ftest-coverage -o "$@" "$<"
	@g++ $(FLAGS) -MM -MT $@ $*.cpp -MF build/deps/$*.d

build/profile/%.o: %.cpp
	g++ -c $(OPTIM) $(FLAGS) -pg -o "$@" "$<"
	@g++ $(FLAGS) -MM -MT $@ $*.cpp -MF build/deps/$*.d

$(BUILD_DIR):
	@mkdir -p $@

# Build test target
build/leekscript-test: $(BUILD_DIR) $(OBJ) $(OBJ_TEST)
	g++ $(FLAGS) -o build/leekscript-test $(OBJ) $(OBJ_TEST) $(LIBS)
	@echo "--------------------------"
	@echo "Build (test) finished!"
	@echo "--------------------------"

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
build/leekscript-coverage: $(BUILD_DIR) $(OBJ_COVERAGE) $(OBJ_TEST)
	g++ $(FLAGS) -fprofile-arcs -ftest-coverage -o build/leekscript-coverage $(OBJ_COVERAGE) $(OBJ_TEST) $(LIBS)
	@echo "--------------------------"
	@echo "Build (coverage) finished!"
	@echo "--------------------------"

# Run tests/
test: build/leekscript-test
	@build/leekscript-test

# Benchmark
benchmark-dir:
	@mkdir -p build/benchmark

build/benchmark/Benchmark.o: benchmark/Benchmark.cpp
	g++ -c $(OPTIM) $(DEPFLAGS) $(FLAGS) -o "$@" "$<"

build/leekscript-benchmark: benchmark-dir build/leekscript $(OBJ_BENCHMARK)
	g++ $(FLAGS) -o build/leekscript-benchmark $(OBJ_BENCHMARK)
	@echo "-------------------------"
	@echo "Benchmark build finished!"
	@echo "-------------------------"

benchmark: build/leekscript-benchmark
	@build/leekscript-benchmark

# Valgrind
valgrind: build/leekscript-test
	valgrind --verbose --track-origins=yes build/leekscript-test

# Travis task, useless in local.
# Build a leekscript docker image, compile, run tests and run cpp-coveralls
# (coverage results are sent to coveralls.io).
travis:
	docker build -t leekscript .
	docker run -e COVERALLS_REPO_TOKEN="$$COVERALLS_REPO_TOKEN" -e TRAVIS_BRANCH="$$TRAVIS_BRANCH" \
	       leekscript /bin/bash -c "cd leekscript; make build/leekscript-coverage && build/leekscript-coverage \
	       && cpp-coveralls -i src/ --gcov-options='-rp'"

# Coverage results with lcov.
# `apt-get install lcov`
coverage: build/leekscript-coverage
	mkdir -p build/html
	lcov --quiet --no-external --rc lcov_branch_coverage=1 --capture --initial --directory build/coverage/src --base-directory src --output-file build/html/app.info
	build/leekscript-coverage
	lcov --quiet --no-external --rc lcov_branch_coverage=1 --capture --directory build/coverage/src --base-directory src --output-file build/html/app.info
	cd build/html; genhtml --ignore-errors source --legend --precision 2 --branch-coverage app.info

# Build with profile flags enabled
build/leekscript-profile: $(BUILD_DIR) $(OBJ_PROFILE) $(OBJ_TEST)
	g++ $(FLAGS) -pg -o build/leekscript-profile $(OBJ_PROFILE) $(OBJ_TEST) $(LIBS)
	@echo "--------------------------"
	@echo "Build (profile) finished!"
	@echo "--------------------------"

profile: build/leekscript-profile
	gprof build/leekscript-profile > profile.stats
	gprof2dot profile.stats | dot -Tpng -o output.png

# Clean every build files by destroying the build/ folder.
clean:
	rm -rf build
	find . -type f -name '*.d' -delete
	@echo "----------------"
	@echo "Project cleaned."
	@echo "----------------"

# Line couning with cloc.
# `apt-get install cloc`
cloc:
	cloc . --exclude-dir=.git,lib,build,doxygen

doc:
	doxygen
	@echo "------------------------"
	@echo "Documentation generated."
	@echo "------------------------"

# Objects dependencies
-include $(DEPS)
