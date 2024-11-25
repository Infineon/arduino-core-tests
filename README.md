# Infineon_arduino_core_unity_test

Repo as shared ardino core tests using unity platform.

## Pre-conditions
Before running Unity tests, please ensure the following pre-conditions are met:

1. **Install Arduino CLI**:
   - Download and install Arduino CLI. You can download the Windows version of Arduino CLI from the following link:
     [Arduino CLI Download Link](https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip)
   - Add `arduino-cli.exe` to your system's PATH environment variable to access it from the command line.

2. **Update submodule**:
   - The Unity test framework is included as submodule in this project. The Unity test framework files should be located in the `unity` folder. Run  `git submodule update --init --recursive` to update them.  If you are nor familiar with submodules, check out: https://git-scm.com/book/en/v2/Git-Tools-Submodules 

## Usage
### Build and Upload Test Code
Currently this test set relies only on arduino-cli for all compilation and uploading. Unity is included as a library, and the starting point for the tests is still in the arduino-specific .ino file.

Please refer to the Makefile and its comments for details.

### Test Architecture
- all test source file naming follow the conventions, e.g.`test_module_connection_testname.cpp`.
- The preprocessor macro / test flag is all uppercase, e.g. `TEST_MODULE_CONNECTION_TESTNAME`.
- The naming of test goup and all related functions removes prefix `test`, e.g. `module_connection_testname`.
- `TEST_IFX()` is used for define test case. There is no naming convention for test case. As long as it explains the functionality to be tested.
![test_architecture](./assets/test_architecture.svg)


### Folder stucture
```
└───unity
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
    │   │   ├───CAN
    │   │   │       test_can_connected2_node1.cpp
    │   │   │       test_can_connected2_node2.cpp
    │   │   │       test_can_single.cpp
    │   │   │
    │   │   └───Wire
    │   │           test_wire_connected1_pingpong.cpp
    │   │           test_wire_connected2_masterpingpong.cpp
    │   │           test_wire_connected2_slaverpingpong.cpp
    │   │
    │   └───utils
    │           utilities.cpp
    │           utilities.hpp
    │
    └───Unity
```

