SRC_DIR := . vm vm/value vm/standard vm/doc parser parser/lexical parser/syntaxic parser/semantic \
parser/value parser/instruction lib benchmark test

BUILD_DIR := $(addprefix build/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/shared/,$(SRC_DIR))
SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ := $(patsubst %.cpp,build/%.o,$(SRC))
OBJ_LIB := $(patsubst %.cpp,build/shared/%.o,$(SRC))

FLAGS := -std=c++11 -O2 -g3 -Wall -Wextra
LIBS := -ljit

all: makedirs leekscript

test: all
	build/leekscript -test

build/%.o: %.cpp
	g++ -c $(FLAGS) -o "$@" "$<"
	
build/shared/%.o: %.cpp
	g++ -c $(FLAGS) -fPIC -o "$@" "$<"
	
makedirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

leekscript: $(OBJ)
	g++ $(FLAGS) -o build/leekscript $(OBJ) $(LIBS)
	@echo "---------------"
	@echo "Build finished!"
	@echo "---------------"
	
lib: makedirs $(OBJ_LIB) 
	g++ $(FLAGS) -shared -o build/libleekscript.so $(OBJ_LIB) $(LIBS)
	@echo "-----------------------"
	@echo "Library build finished!"
	@echo "-----------------------"
	
install: lib
	cp build/libleekscript.so /usr/lib/
	@echo "Library installed!"
	
clean:
	rm -rf build
	@echo "----------------"
	@echo "Project cleaned."
	@echo "----------------"

