# Vairables for project configuration
FQBN  ?=
PORT  ?=
TESTS ?=
UNITY_PATH ?= Unity
BAUD_RATE ?= 115200
SERIAL ?=
Q?=@
ENABLE_SYNC ?= 1

.PHONY: print_args clean check_unity_path unity flash compile upload monitor

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
unity: print_args clean check_unity_path
	$(Q) find $(UNITY_PATH) -name '*.[hc]' \( -path '*extras*' -a -path '*src*' -or -path '*src*' -a \! -path '*example*' \) -exec \cp {} build \;
	$(Q) find src/utils -name '*.[hc]*' -exec \cp {} build \;
	$(Q) find src -maxdepth 1 -name '*.[hc]*' -exec \cp {} build \;
	$(Q) find ../../tests -maxdepth 1 -name 'test_config.*' -exec \cp {} build \;
	$(Q) cp src/test_main.ino build/build.ino

# Helper to extract the second word from the target name
CATEGORY = $(word 2, $(subst _, ,$@))

# Test target example
test_%: unity
	$(eval CATEGORY := $(word 2, $(subst _, ,$@)))
	$(Q) cp src/corelibs/$(CATEGORY)/$@.cpp build 2>/dev/null || true
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

## SPI tests targets
test_spi_connected1_loopback: TESTS=-DTEST_SPI_CONNECTED1_LOOPBACK
test_spi_connected2_masterpingpong: TESTS=-DTEST_SPI_CONNECTED2_MASTERPINGPONG
test_spi_connected2_slavepingpong:  TESTS=-DTEST_SPI_CONNECTED2_SLAVEPINGPONG


## OneWire tests targets
test_onewire_DS18x20: TESTS=-DTEST_ONEWIRE_DS18x20

# Arduino-cli commands

# For WSL and Windows :
# download arduino-cli from : https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip
# .PHONY: flash compile upload monitor

compile:
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to compile Arduino sketches !")
else
	arduino-cli compile --clean --log --warnings all --fqbn $(FQBN) \
		                    --build-property compiler.c.extra_flags="\"-DUNITY_INCLUDE_CONFIG_H=1\"" \
		--build-property compiler.cpp.extra_flags="$(TESTS) $(if $(filter 1,$(ENABLE_SYNC)),-DENABLE_SYNC,)" \
		build


# 	                        --build-property compiler.c.extra_flags="\"-DUNITY_INCLUDE_CONFIG_H=1\"" \

# --build-property compiler.c.extra_flags="\"-DUNITY_INCLUDE_CONFIG_H=1\" -I/myLocalWorkingDir/extras/arduino-core-api -I/myLocalWorkingDir/extras/arduino-core-api/api" \
# --build-property compiler.cpp.extra_flags="$(TESTS) -I/myLocalWorkingDir/extras/arduino-core-api -I/myLocalWorkingDir/extras/arduino-core-api/api" \
# --build-property compiler.c.extra_flags="\"-DUNITY_INCLUDE_CONFIG_H=1\"" \
# --build-property compiler.cpp.extra_flags="$(TESTS)" \

endif


upload:	compile
ifeq ($(PORT),)
	$(error "Must set variable PORT (Windows port naming convention, ie COM16) in order to be able to flash Arduino sketches !")
endif
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to flash Arduino sketches !")
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
	$(error "Must set variable PORT (Windows port naming convention, ie COM16) in order to be able to flash Arduino sketches !")
endif
	arduino-cli monitor -c baudrate=115200 -p $(PORT)
