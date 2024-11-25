# Vairables for project configuration
FQBN  ?=
PORT  ?=
TESTS ?=
UNITY_PATH ?=
BAUD_RATE ?= 115200

# Info for debugging
$(info FQBN : $(FQBN))
$(info PORT : $(PORT))
$(info UNITY_PATH : $(UNITY_PATH))
$(info BAUD_RATE : $(BAUD_RATE))


# Clean and create build directory for arduino compilation
clean:
	-rm -rf build/*

build: clean
	mkdir -p build

# CAN module

## CAN Examples targets
CANReceiver: build
	cp ../../libraries/CAN/examples//CANReceiver/CANReceiver.ino build/build.ino

CANReceiverCallback: build
	cp examples/CANReceiverCallback/CANReceiverCallback.ino build/build.ino

CANSender: build
	cp examples/CANSender/CANSender.ino build/build.ino

CANLoopBack: build
	cp examples/CANLoopBack/CANLoopBack.ino build/build.ino


## CAN tests targets
test_can_single: TESTS=-DTEST_CAN_SINGLE
test_can_connected2_node1: TESTS=-DTEST_CAN_CONNECTED2_NODE1
test_can_connected2_node2: TESTS=-DTEST_CAN_CONNECTED2_NODE2

test_can_single\
test_can_connected2_node1\
test_can_connected2_node2\
\
: cp_unity_can flash

cp_unity_can: build
ifeq ($(UNITY_PATH),)
    $(error "Must set variable UNITY_PATH in order to be able to compile Arduino unit tests !")
else
	$(call FIND_TEST_AND_COPY, src/corelibs/CAN)
endif

# Wire(IIC) module

## IIC tests targets
wire_connected1_pingpong: TESTS=-DTEST_WIRE_CONNECTED1_PINGPONG
wire_connected2_masterpingpong: TESTS=-DTEST_WIRE_CONNECTED2_MASTERPINGPONG
wire_connected2_slavepingpong:  TESTS=-DTEST_WIRE_CONNECTED2_SLAVEPINGPONG

wire_connected1_pingpong \
wire_connected2_masterpingpong \
wire_connected2_slavepingpong \
\
: cp_unity_i2c flash

cp_unity_i2c: build
ifeq ($(UNITY_PATH),)
    $(error "Must set variable UNITY_PATH in order to be able to compile Arduino unit tests !")
else
	$(call FIND_TEST_AND_COPY, src/corelibs/Wire)
endif


# Copy Unity files and core libs to build directory
define FIND_TEST_AND_COPY
    find $(UNITY_PATH) -name '*.[hc]' \( -path '*extras*' -a -path '*src*' -or -path '*src*' -a \! -path '*example*' \) -exec \cp {} build \;
    find $(1) -name '*.[hc]*' -exec \cp {} build \;
    find src/utils -name '*.[hc]*' -exec \cp {} build \;
    find src -maxdepth 1 -name '*.[hc]*' -exec \cp {} build \;
    cp src/Test_main.ino build/build.ino
endef

cp_unity_core: build
ifeq ($(UNITY_PATH),)
    $(error "Must set variable UNITY_PATH in order to be able to compile Arduino unit tests !")
else
	$(call FIND_TEST_AND_COPY, src/corelibs)
endif



# Arduino-cli commands

# For WSL and Windows :
# download arduino-cli.exe from : https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip
compile:
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to compile Arduino sketches !")
else
# CAUTION : only use '=' when assigning values to vars, not '+='
	arduino-cli.exe compile \
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
	arduino-cli.exe compile \
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
	arduino-cli.exe upload \
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
	arduino-cli.exe monitor \
						-c baudrate=$(BAUD_RATE) \
						-p $(PORT) \
						--fqbn $(FQBN)
endif