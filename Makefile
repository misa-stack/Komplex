BUILD_DIR ?= build
BUILD_TYPE ?= Release
JOBS ?= 4
.PHONY: all configure test run debug clean
all: configure
	cmake --build $(BUILD_DIR) --parallel $(JOBS)
	cp $(BUILD_DIR)/program ./program
configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
test: all
	ctest --test-dir $(BUILD_DIR) --output-on-failure
run: all
	./program
debug:
	$(MAKE) BUILD_DIR=build-debug BUILD_TYPE=Debug all
clean:
	cmake -E rm -rf $(BUILD_DIR)
