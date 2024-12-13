# Vairables for project configuration
FQBN  ?=
PORT  ?=
TESTS ?=
UNITY_PATH ?= Unity
BAUD_RATE ?= 115200

# Info for debugging
$(info FQBN : $(FQBN))
$(info PORT : $(PORT))
$(info UNITY_PATH : $(UNITY_PATH))
$(info BAUD_RATE : $(BAUD_RATE))


.PHONY: flash compile upload monitor

# Clean and create build directory for arduino compilation
clean:
	-rm -rf build/*

build: clean
	mkdir -p build

# Check if UNITY_PATH variable is set
check_unity_path:
ifndef UNITY_PATH
    $(error "Must set variable UNITY_PATH in order to be able to compile Arduino unit tests!")
endif


# Copy common files and test-specific files to build directory
define COPY_COMMON_FILES
    find $(UNITY_PATH) -name '*.[hc]' \( -path '*extras*' -a -path '*src*' -or -path '*src*' -a \! -path '*example*' \) -exec \cp {} build \;
    find src/utils -name '*.[hc]*' -exec \cp {} build \;
    find src -maxdepth 1 -name '*.[hc]*' -exec \cp {} build \;
    cp src/test_main.ino build/build.ino
endef

# Copy specific target C file to build directory
define COPY_TARGET_FILE
	find src/corelibs/$(call strip,$(1)) -name '$(call strip,$(2)).cpp' -exec \cp {} build \;
endef

# create build folder for test target	
test_%: build check_unity_path
	$(call COPY_COMMON_FILES)
	$(call COPY_TARGET_FILE, $(shell echo $(@:test_%=%) | cut -d"_" -f1), $(@))
	$(MAKE) compile TESTS=$(TESTS)
 
# UART tests targets
test_uart_connected2_tx: TESTS=-DTEST_UART_CONNECTED2_TX
test_uart_connected2_rx: TESTS=-DTEST_UART_CONNECTED2_RX


## CAN tests targets
test_can_single: TESTS=-DTEST_CAN_SINGLE
test_can_connected2_node1: TESTS=-DTEST_CAN_CONNECTED2_NODE1
test_can_connected2_node2: TESTS=-DTEST_CAN_CONNECTED2_NODE2

## IIC tests targets
test_wire_connected1_pingpong: TESTS=-DTEST_WIRE_CONNECTED1_PINGPONG
test_wire_connected2_masterpingpong: TESTS=-DTEST_WIRE_CONNECTED2_MASTERPINGPONG
test_wire_connected2_slavepingpong:  TESTS=-DTEST_WIRE_CONNECTED2_SLAVEPINGPONG

# Arduino-cli commands

# For WSL and Windows :
# download arduino-cli.exe from : https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip
.PHONY: flash compile upload monitor

compile:
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to compile Arduino sketches !")
else
# CAUTION : only use '=' when assigning values to vars, not '+='
	arduino-cli compile \
						--config-file ~/.arduino15/arduino-cli.yaml \
						--clean \
						--log \
						--warnings all \
						--fqbn $(FQBN) \
						--build-property "compiler.c.extra_flags=\"-DUNITY_INCLUDE_CONFIG_H=1\"" \
						--build-property compiler.cpp.extra_flags="$(TESTS)" \
						--export-binaries \
					build
endif


compileLTO:
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to compile Arduino sketches !")
else
# compiler.c.extra_flags : switch to -std=c23 whenever XMCLib is conforming; currently neither c99 nor c11 work !
# CAUTION : only use '=' when assigning values to vars, not '+='
	arduino-cli compile \
						 --config-file ~/.arduino15/arduino-cli.yaml \
						--clean \
						--log \
						--warnings all \
						--fqbn $(FQBN) \
						--build-property compiler.c.extra_flags="\"-DUNITY_INCLUDE_CONFIG_H=1\" -DNDEBUG -flto -fno-fat-lto-objects -Wextra -Wall -Wfloat-equal -Wconversion -Wredundant-decls -Wswitch-default -Wdouble-promotion -Wpedantic -Wunreachable-code -fanalyzer -std=c20 " \
						--build-property compiler.cpp.extra_flags="$(TESTS) -DNDEBUG -flto -fno-fat-lto-objects -Wextra -Wall -Wfloat-equal -Wconversion -Wredundant-decls -Wswitch-default -Wdouble-promotion -Wpedantic -Wunreachable-code -fanalyzer -std=c++20 " \
						--build-property compiler.ar.cmd=arm-none-eabi-gcc-ar \
						--build-property compiler.libraries.ldflags=-lstdc++ \
						--build-property compiler.arm.cmsis.path="-isystem{compiler.xmclib_include.path}/XMCLib/inc -isystem{compiler.dsp_include.path} -isystem{compiler.nn_include.path} -isystem{compiler.cmsis_include.path} -isystem{compiler.xmclib_include.path}/LIBS -isystem{build.variant.path} -isystem{build.variant.config_path}" \
						--build-property compiler.usb.path="-isystem{runtime.platform.path}/cores/usblib -isystem{runtime.platform.path}/cores/usblib/Common -isystem{runtime.platform.path}/cores/usblib/Class -isystem{runtime.platform.path}/cores/usblib/Class/Common -isystem{runtime.platform.path}/cores/usblib/Class/Device -isystem{runtime.platform.path}/cores/usblib/Core -isystem{runtime.platform.path}/cores/usblib/Core/XMC4000" \
						--export-binaries \
					build
endif


upload:	compile
ifeq ($(PORT),)
	$(error "Must set variable PORT (Windows port naming convention, ie COM16) in order to be able to flash Arduino sketches !")
endif
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to flash Arduino sketches !")
else
	arduino-cli upload \
						-p $(PORT) \
						--fqbn $(FQBN) \
						--verbose \
					build
endif


flash: compile upload


monitor:
ifeq ($(PORT),)
	$(error "Must set variable PORT (Windows port naming convention, ie COM16) in order to be able to flash Arduino sketches !")
endif
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to flash Arduino sketches !")
else
	arduino-cli monitor \
						-c baudrate=$(BAUD_RATE) \
						-p $(PORT) \
						--fqbn $(FQBN)
endif