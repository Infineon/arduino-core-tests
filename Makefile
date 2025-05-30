# Vairables for project configuration
FQBN  ?=
PORT  ?=
TESTS ?=
UNITY_PATH ?= Unity
BAUD_RATE ?= 115200
Q?=@

print_args:
	@:
# Info for debugging
	$(info )
	$(info Test configuration)
	$(info ----------------------------------)
	$(info FQBN      : $(FQBN))
	$(info PORT      : $(PORT))
	$(info BAUD_RATE : $(BAUD_RATE))
	$(info ----------------------------------)
	$(info )


.PHONY: flash compile upload monitor

# Clean and create build directory for arduino compilation
clean:
	$(Q) -rm -rf build/*

create_build_dir: clean
	$(Q) mkdir -p build

# Check if UNITY_PATH variable is set
check_unity_path:
ifndef UNITY_PATH
    $(error "Must set variable UNITY_PATH in order to be able to compile Arduino unit tests!")
endif


# Copy common files and test-specific files to build directory
define COPY_COMMON_FILES
    $(Q) find $(UNITY_PATH) -name '*.[hc]' \( -path '*extras*' -a -path '*src*' -or -path '*src*' -a \! -path '*example*' \) -exec \cp {} build \;
    $(Q) find src/utils -name '*.[hc]*' -exec \cp {} build \;
	$(Q) find src -maxdepth 1 -name '*.[hc]*' -exec \cp {} build \;
	$(Q) find ../ -maxdepth 1 -name 'test_config.h' -exec \cp {} build \;
    $(Q) cp src/test_main.ino build/build.ino
endef

# Copy specific target C file to build directory
define COPY_TARGET_FILE
	$(Q) find src/corelibs/$(call strip,$(1)) -name '$(call strip,$(2)).cpp' -exec \cp {} build \;
endef

# create build folder for test target	
test_%: create_build_dir check_unity_path print_args
	$(Q) $(call COPY_COMMON_FILES)
	$(Q) $(call COPY_TARGET_FILE, $(shell echo $(@:test_%=%) | cut -d"_" -f1), $(@))
	$(info )
	$(info Testing $(@:test_%=%)...)
	$(Q) $(MAKE) flash TESTS=$(TESTS) --no-print-directory
	$(Q) $(MAKE) monitor PORT=$(PORT) FQBN=$(FQBN) --no-print-directory

# UART tests targets
test_uart_connected2_tx: TESTS=-DTEST_UART_CONNECTED2_TX
test_uart_connected2_rx: TESTS=-DTEST_UART_CONNECTED2_RX
test_uart_rx: TESTS=-DTEST_UART_RX
test_uart_tx: TESTS=-DTEST_UART_TX

# Time tests targets
test_time_single: TESTS=-DTEST_TIME_SINGLE

# Digital IO tests targets
test_digitalio_single: TESTS=-DTEST_DIGITALIO_SINGLE

# Analog IO tests targets
test_analogio_adc: TESTS=-DTEST_ANALOGIO_ADC
test_analogio_pwm: TESTS=-DTEST_ANALOGIO_PWM

# GPIO Interrupts tests targets
test_interrupts_single: TESTS=-DTEST_INTERRUPTS_SINGLE

## CAN tests targets
test_can_single: TESTS=-DTEST_CAN_SINGLE
test_can_connected2_node1: TESTS=-DTEST_CAN_CONNECTED2_NODE1
test_can_connected2_node2: TESTS=-DTEST_CAN_CONNECTED2_NODE2

## IIC tests targets
test_wire_connected1_pingpong: TESTS=-DTEST_WIRE_CONNECTED1_PINGPONG
test_wire_connected2_masterpingpong: TESTS=-DTEST_WIRE_CONNECTED2_MASTERPINGPONG
test_wire_connected2_slavepingpong:  TESTS=-DTEST_WIRE_CONNECTED2_SLAVEPINGPONG

## WiFi tests targets
test_wifi_sta: TESTS=-DTEST_WIFI_STA
test_wifi_ap: TESTS=-DTEST_WIFI_AP
test_wifi_client: TESTS=-DTEST_WIFI_CLIENT
test_wifi_server: TESTS=-DTEST_WIFI_SERVER
test_wifi_extras: TESTS=-DTEST_WIFI_EXTRAS
test_wifi_exceptions: TESTS=-DTEST_WIFI_EXCEPTIONS
test_wifi_udp_client: TESTS=-DTEST_WIFI_UDP_CLIENT
test_wifi_udp_server: TESTS=-DTEST_WIFI_UDP_SERVER

## SPI tests targets
test_spi_connected1_loopback: TESTS=-DTEST_SPI_CONNECTED1_LOOPBACK
test_spi_connected2_masterpingpong: TESTS=-DTEST_SPI_CONNECTED2_MASTERPINGPONG
test_spi_connected2_slavepingpong:  TESTS=-DTEST_SPI_CONNECTED2_SLAVEPINGPONG

## Tone tests targets
test_tone_no_tone: TESTS=-DTEST_TONE_NO_TONE

## Pulse tests targets
test_pulse_board1: TESTS=-DTEST_PULSE_BOARD1
test_pulse_board2: TESTS=-DTEST_PULSE_BOARD2

# Arduino-cli commands

# For WSL and Windows :
# download arduino-cli from : https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip
.PHONY: flash compile upload monitor

compile:
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to compile Arduino sketches !")
else
# CAUTION : only use '=' when assigning values to vars, not '+='
	$(info Compiling test...)
	$(Q) arduino-cli compile \
						--clean \
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
	$(Q) arduino-cli compile \
						--clean \
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
	$(Q) arduino-cli upload \
					-p $(PORT) \
					--fqbn $(FQBN) \
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
	$(info Uploading test...)
	$(Q) arduino-cli monitor \
						-c baudrate=$(BAUD_RATE) \
						-p $(PORT) \
						--fqbn $(FQBN)
endif
