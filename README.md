# Infineon_arduino_core_unity_test

Repo as shared ardino core tests using unity platform.

## Scope

In this table you find an overview of the Arduino cores the tests are currently running on.
- :white_check_mark: indicates the test is supported on the platform.
- :question: indicates that the support needs to be verified.
- :x: indicates the test needs to be implemented on the platform or modified to run on the platform.
- :no_entry_sign: indicates the test is not relevant for the platform.

| Test                           |        PSOC6         |       XMC3.x        |       XMC4.x        |
|--------------------------------|:--------------------:|:-------------------:|:-------------------:|
| analogio_adc                   | :white_check_mark:   |     :question:      |     :question:      |
| analogio_dac                   | :white_check_mark:   |     :question:      |     :question:      |
| analogio_pwm                   | :white_check_mark:   |     :question:      |     :question:      |
| can_connected2_node1           |   :no_entry_sign:    | :white_check_mark:  |     :question:      |
| can_connected2_node2           |   :no_entry_sign:    | :white_check_mark:  |     :question:      |
| can_single                     |   :no_entry_sign:    | :white_check_mark:  |     :question:      |
| digitalio_single               | :white_check_mark:   | :white_check_mark:  |     :question:      |
| interrupts_single              | :white_check_mark:   |     :question:      |     :question:      |
| onewire_DS18x20                | :white_check_mark:   | :white_check_mark:  |     :question:      |
| pulse_board1                   | :white_check_mark:   |     :question:      |     :question:      |
| pulse_board2                   | :white_check_mark:   |     :question:      |     :question:      |
| random                         | :white_check_mark:   |     :question:      |     :question:      |
| spi_connected1_loopback        | :white_check_mark:   |     :question:      |     :question:      |
| spi_connected2_masterpingpong  | :white_check_mark:   |     :question:      |     :question:      |
| spi_connected2_slavepingpong   | :white_check_mark:   |     :question:      |     :question:      |
| time_single                    | :white_check_mark:   |     :question:      |     :question:      |
| tone_no_tone                   | :white_check_mark:   |     :question:      |     :question:      |
| uart_connected2_rx             | :white_check_mark:   |     :question:      |     :question:      |
| uart_connected2_tx             | :white_check_mark:   |     :question:      |     :question:      |
| uart_rx                        | :white_check_mark:   |     :question:      |     :question:      |
| uart_tx                        | :white_check_mark:   |     :question:      |     :question:      |
| wifi_ap                        | :white_check_mark:   |   :no_entry_sign:   |   :no_entry_sign:   |
| wifi_client                    | :white_check_mark:   |   :no_entry_sign:   |   :no_entry_sign:   |
| wifi_exceptions                | :white_check_mark:   |   :no_entry_sign:   |   :no_entry_sign:   |
| wifi_extras                    | :white_check_mark:   |   :no_entry_sign:   |   :no_entry_sign:   |
| wifi_server                    | :white_check_mark:   |   :no_entry_sign:   |   :no_entry_sign:   |
| wifi_sta                       | :white_check_mark:   |   :no_entry_sign:   |   :no_entry_sign:   |
| wifi_udp_client                | :white_check_mark:   |   :no_entry_sign:   |   :no_entry_sign:   |
| wifi_udp_server                | :white_check_mark:   |   :no_entry_sign:   |   :no_entry_sign:   |
| wire_connected1_pingpong       | :white_check_mark:   | :white_check_mark:  |     :question:      |
| wire_connected2_masterpingpong | :white_check_mark:   | :white_check_mark:  |     :question:      |
| wire_connected2_slavepingpong  | :white_check_mark:   | :white_check_mark:  |     :question:      |

## Pre-conditions
Before running Unity tests, please ensure the following pre-conditions are met:

### Install Arduino CLI:
   - Download and install Arduino CLI: https://arduino.github.io/arduino-cli/latest/installation/
      You may need to add it to your system's PATH environment variable to access it from terminal.
### Install Make:
   - Install GNU make: https://www.gnu.org/software/make/. We use currently Makefile to manage how we execute different tests.
### Update submodule:
   - The Unity test framework is included as submodule in this project. The Unity test framework files should be located in the `unity` folder. Run  `git submodule update --init --recursive` to update them.  If you are nor familiar with submodules, check out: https://git-scm.com/book/en/v2/Git-Tools-Submodules 

## Usage
### Build and Upload Test Code
Currently this test set relies only on arduino-cli for all compilation and uploading. Unity is included as a library, and the starting point for the tests is still in the arduino-specific .ino file.

Please refer to the Makefile and its comments for details.

### Test Architecture
- all test source file naming follow the conventions, e.g.`test_module_connection_testname.cpp`. The make target also have same name, e.g. `test_module_connection_testname`. 
- The preprocessor macro / test flag is all uppercase, e.g. `TEST_MODULE_CONNECTION_TESTNAME`.
- The naming of test goup and all related functions removes prefix `test`, e.g. `module_connection_testname`.
- `TEST_IFX()` is used for define test case. There is no naming convention for test case. As long as it explains the functionality to be tested.
![test_architecture](./assets/test_architecture.svg)


### Folder stucture
```
└───arduino-core-tests
    │   .gitignore
    │   .gitmodules
    │   LICENSE
    │   Makefile // define test target and preprocessor macros/ test flag. 
    │   README.md
    │
    ├───build
    │
    ├───src
    │   │   test_common_includes.h
    │   │   test_main.ino // starting point of tests,
    │   │                 // selecting the test group according to the 
    │   │                 // test flag define in Makefile.
    │   │   unity_config.h
    │   │   unity_ifx.cpp
    │   │   unity_ifx.h
    │   │
    │   ├───corelibs // Tests for all Built-in library and core modules
    │   │   ├───module // module name, lower case!
    │   │   │       test_module_connection_name1.cpp
    │   │   │       test_module_connection_name2.cpp
    │   │   │       test_module_single.cpp
    │   │   │
    │   │   ├───can 
    │   │   │       test_can_connected2_node1.cpp
    │   │   │       test_can_connected2_node2.cpp
    │   │   │       test_can_single.cpp
    │   │   │
    │   │   ├───wire 
    │   │   │       test_wire_connected1_pingpong.cpp
    │   │   │       test_wire_connected2_masterpingpong.cpp
    │   │   │       test_wire_connected2_slavepingpong.cpp
    │   │   │
    │   │   └───....
    │   │           
    │   │    
    │   │
    │   └───utils
    │           utilities.cpp
    │           utilities.hpp
    │
    └───Unity
└───hil-unity-checklist.yaml // project specific test list to run in CI/CD HIL setup
└───test_config.h // project specific test configurations, e.g. test pins
```

