SRC_DIR := . vm vm/value vm/standard parser parser/lexical parser/syntaxic parser/semantic \
parser/value parser/instruction lib benchmark test

BUILD_DIR := $(addprefix build/,$(SRC_DIR))
SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ := $(patsubst %.cpp,build/%.o,$(SRC))

all: makedirs leekscript

build/%.o: %.cpp
	g++ -std=c++0x -I/usr/local/lib -I/var/lib -O3 -g3 -Wall -c -ljit -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

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
	
