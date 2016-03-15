SRC_DIR := . vm vm/value vm/standard vm/doc parser parser/lexical parser/syntaxic parser/semantic \
parser/value parser/instruction lib benchmark test

BUILD_DIR := $(addprefix build/,$(SRC_DIR))
SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ := $(patsubst %.cpp,build/%.o,$(SRC))

all: makedirs leekscript

test: all
	build/leekscript -test

build/%.o: %.cpp
	g++ -c -std=c++11 -O3 -g3 -Wall -Wextra -ljit -o "$@" "$<"

makedirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

leekscript: $(OBJ)
	g++ -std=c++11 -o build/leekscript $(OBJ) -ljit
	@echo "---------------"
	@echo "Build finished!"
	@echo "---------------"

clean:
	rm -rf build
	@echo "----------------"
	@echo "Project cleaned."
	@echo "----------------"

