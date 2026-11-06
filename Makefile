.PHONY: all clean build prog setup

BOARD ?= nucleo_f411re
BUILD_DIR ?= build
PYTHON ?= $(abspath .venv/bin/python)
HEX ?= $(abspath $(BUILD_DIR)/zephyr/zephyr.hex)
CMAKE_ARGS ?=

all: clean build

clean:
	rm -rf "$(BUILD_DIR)"

build:
	cmake -S . -B "$(BUILD_DIR)" -G Ninja -DBOARD="$(BOARD)" -DPython3_EXECUTABLE="$(PYTHON)" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON $(CMAKE_ARGS)
	cmake --build "$(BUILD_DIR)"

prog: 
	@test -f "$(HEX)" || { echo "Build not found: $(HEX)" >&2; exit 1; }
	STM32_Programmer_CLI -c port=SWD mode=UR -w "$(HEX)" -v -rst

setup:
	bash setup.sh
