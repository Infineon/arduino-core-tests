# Makefile for running unit tests and handling Arduino builds
# Variables for project configuration
FQBN  ?=
PORT  ?=
TESTS ?=
UNITY_PATH ?= Unity
BAUD_RATE ?= 115200
SERIAL ?=
Q?=@
ENABLE_SYNC ?= 1

.PHONY: print_args clean check_unity_path unity flash compile upload monitor

# Default target: Show help when no target is specified
.DEFAULT_GOAL := help

help:
	@echo "============================================================================================================"
	@echo "Makefile Usage Guide"
	@echo "============================================================================================================"
	@echo ""
	@echo "Usage:"
	@echo "  make [TARGET] [VARIABLES]"
	@echo ""
	@echo "Available Targets:"
	@echo "  help                     Show this help guide and documentation"
	@echo "  list_tests               List all available test targets"
	@echo "  print_args               Print current configuration variables for debugging"
	@echo "  clean                    Clean the build directory (removes all intermediate files)"
	@echo "  prepare_test_environment Prepare the environment by copying common, test-specific files"
	@echo "  compile                  Compile the sketch for the specified board"
	@echo "  upload                   Upload the compiled sketch to the board via the specified port"
	@echo "  flash                    Perform both 'compile' and 'upload' in one go"
	@echo "  monitor                  Open the serial monitor for the given board and port"
	@echo ""
	@echo "Test Targets:"
	@echo "  test_<test_name>         Runs a specific test by preparing the test environment, compiling, uploading the firmware,"
	@echo "                           and running it on the target board."
	@echo "                           Example: test_digitalio_single or test_wifi_sta"
	@echo ""
	@echo "Variables:"
	@echo "  FQBN=<board_name>        Fully Qualified Board Name (default: empty). Used to specify the microcontroller board."
	@echo "                           Example: infineon:psoc6:cy8ckit_062s2_ai"
	@echo ""
	@echo "  PORT=<serial_port>       Serial port to connect to the board (default: empty)."
	@echo "                           Examples: /dev/ttyUSB0"
	@echo ""
	@echo "  ENABLE_SYNC=<0|1>        Enable (1) or disable (0) synchronization for multi boards (default: 1)."
	@echo "                           This is primarily used in CI/CD HIL checks."
	@echo ""
	@echo "  BAUD_RATE=<rate>         Baud rate for serial communication (default: 115200)."
	@echo "                           Example: BAUD_RATE=9600"
	@echo ""
	@echo "  SERIAL=<serial_number>   Optional serial number for advanced port identification (default: empty)."
	@echo "                           Example: SERIAL=123456789ABC"
	@echo ""
	@echo "Examples:"
	@echo "  1. Prepare the test environment:"
	@echo "     make prepare_test_environment"
	@echo ""
	@echo "  2. Run a specific test for DigitalIO with board and port:"
	@echo "     make FQBN=infineon:psoc6:cy8ckit_062s2_ai PORT=/dev/ttyUSB0 test_digitalio_single"
	@echo ""
	@echo "  3. Run a test with synchronization disabled:"
	@echo "     make FQBN=infineon:psoc6:cy8ckit_062s2_ai PORT=/dev/ttyUSB0 test_digitalio_single ENABLE_SYNC=0"
	@echo ""
	@echo "  4. Open the serial monitor with a specific port:"
	@echo "     make PORT=/dev/ttyUSB0 monitor"
	@echo ""
	@echo "  5. Combine running a test and opening the serial monitor:"
	@echo "     make FQBN=infineon:psoc6:cy8ckit_062s2_ai PORT=/dev/ttyUSB0 test_digitalio_single monitor"
	@echo ""
	@echo "============================================================================================================"
	@echo ""

list_tests:
	@echo "Available Test Targets:"
	@echo ""
	@grep -E '^test_[a-zA-Z0-9_]+:' $(MAKEFILE_LIST) | cut -d':' -f1 | sort
	@echo ""

print_args:
#	@:
# Info for debugging
	$(info )
	$(info Test configuration)
	$(info ----------------------------------)
	$(info FQBN      	: $(FQBN))
	$(info UNITY_PATH  	: $(UNITY_PATH))
	$(info PORT      	: $(PORT))
	$(info SERIAL    	: $(SERIAL))
	$(info BAUD_RATE 	: $(BAUD_RATE))
	$(info ----------------------------------)
	$(info )

# Clean and create build directory for arduino compilation
clean:
	$(Q) -rm -rf build/*
	$(Q) mkdir -p build

# Check if UNITY_PATH variable is set
check_unity_path:
ifndef UNITY_PATH
	$(error "Must set variable UNITY_PATH in order to be able to compile Arduino unit tests!")
endif

# Copy common files and test-specific files to build directory
prepare_test_environment: print_args clean check_unity_path
	$(Q) find $(UNITY_PATH) -name '*.[hc]' \( -path '*extras*' -a -path '*src*' -or -path '*src*' -a \! -path '*example*' \) -exec \cp {} build \;
	$(Q) find src/utils -name '*.[hc]*' -exec \cp {} build \;
	$(Q) find src -maxdepth 1 -name '*.[hc]*' -exec \cp {} build \;
	$(Q) find ../../tests -maxdepth 1 -name '*.[hc]*' -exec \cp {} build \;
	$(Q) cp src/test_main.ino build/build.ino

# Test target example
test_%: prepare_test_environment
	$(eval CATEGORY := $(word 2, $(subst _, ,$@)))
	$(Q) cp src/corelibs/$(CATEGORY)/$@.cpp build 2>/dev/null || true
	@if [ -z "$(TESTS)" ]; then \
        echo "Must set variable TESTS in order to be able to flash a specific arduino core test !"; \
        exit 1; \
    fi
	$(MAKE) flash TESTS=$(TESTS)

# UART tests targets
test_uart_connected2_tx: TESTS=-DTEST_UART_CONNECTED2_TX
test_uart_connected2_rx: TESTS=-DTEST_UART_CONNECTED2_RX
test_uart_rx: TESTS=-DTEST_UART_RX
test_uart_tx: TESTS=-DTEST_UART_TX

# Time tests targets
test_time_single: TESTS=-DTEST_TIME_SINGLE

# Digital IO tests targets
test_digitalio_single: TESTS=-DTEST_DIGITALIO_SINGLE

# Random tests targets
test_random: TESTS=-DTEST_RANDOM

# Analog IO tests targets
test_analogio_adc: TESTS=-DTEST_ANALOGIO_ADC
test_analogio_pwm: TESTS=-DTEST_ANALOGIO_PWM
test_analogio_dac: TESTS=-DTEST_ANALOGIO_DAC	

# Advanced IO tests targets
test_tone_no_tone: TESTS=-DTEST_TONE_NO_TONE
test_pulse_board1: TESTS=-DTEST_PULSE_BOARD1
test_pulse_board2: TESTS=-DTEST_PULSE_BOARD2

# GPIO Interrupts tests targets
test_interrupts_single: TESTS=-DTEST_INTERRUPTS_SINGLE

## CAN tests targets
test_can_single: TESTS=-DTEST_CAN_SINGLE
test_can_connected2_node1: TESTS=-DTEST_CAN_CONNECTED2_NODE1
test_can_connected2_node2: TESTS=-DTEST_CAN_CONNECTED2_NODE2

## Wire tests targets
test_wire_connected1_pingpong: TESTS=-DTEST_WIRE_CONNECTED1_PINGPONG
test_wire_connected2_masterpingpong: TESTS=-DTEST_WIRE_CONNECTED2_MASTERPINGPONG
test_wire_connected2_slavepingpong:  TESTS=-DTEST_WIRE_CONNECTED2_SLAVEPINGPONG

## WiFi tests targets
test_wifi_sta: TESTS=-DTEST_WIFI_STA
test_wifi_ap: TESTS=-DTEST_WIFI_AP
test_wifi_client: TESTS=-DTEST_WIFI_CLIENT
test_wifi_server: TESTS=-DTEST_WIFI_SERVER
test_wifi_udp_client: TESTS=-DTEST_WIFI_UDP_CLIENT
test_wifi_udp_server: TESTS=-DTEST_WIFI_UDP_SERVER
test_wifi_extras: TESTS=-DTEST_WIFI_EXTRAS
test_wifi_exceptions: TESTS=-DTEST_WIFI_EXCEPTIONS

## SPI tests targets
test_spi_connected1_loopback: TESTS=-DTEST_SPI_CONNECTED1_LOOPBACK
test_spi_connected2_masterpingpong: TESTS=-DTEST_SPI_CONNECTED2_MASTERPINGPONG
test_spi_connected2_slavepingpong:  TESTS=-DTEST_SPI_CONNECTED2_SLAVEPINGPONG

.PHONY: install_onewire_libs
install_onewire_libs:
	$(Q) arduino-cli lib update-index
	$(Q) arduino-cli lib install "OneWire"

## OneWire tests targets
test_onewire_DS18x20: TESTS=-DTEST_ONEWIRE_DS18x20
test_onewire_DS18x20: install_onewire_libs

# Arduino-cli commands

# For WSL and Windows :
# download arduino-cli from : https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip
# .PHONY: flash compile upload monitor

compile:
ifeq ($(FQBN),)
	 $(error "Error: FQBN must be set to compile Arduino sketches!")
else
	arduino-cli compile --clean --log --warnings all --fqbn $(FQBN) \
		--build-property compiler.c.extra_flags="\"-DUNITY_INCLUDE_CONFIG_H=1\"" \
		--build-property compiler.cpp.extra_flags="$(TESTS) $(if $(filter 1,$(ENABLE_SYNC)),-DENABLE_SYNC,)" \
		build
endif

upload:	compile
ifeq ($(PORT),)
	$(error "Error: PORT must be set to upload Arduino sketches!")
endif
ifeq ($(FQBN),)
	$(error "Error: FQBN must be set to upload Arduino sketches!")
else
ifeq ($(SERIAL),)
	arduino-cli upload --log --log-level info -v -p $(PORT) --fqbn $(FQBN) build
else
	arduino-cli upload --log --log-level info -v -p $(PORT) --fqbn $(FQBN) --upload-property upload.port.properties.serialNumber=$(SERIAL) build
endif
endif

flash: compile upload

monitor:
ifeq ($(PORT),)
	$(error "Error: PORT must be set to open the serial monitor!")
endif
	arduino-cli monitor -c baudrate=$(BAUD_RATE) -p $(PORT)
