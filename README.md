# arduino-core-tests

This repository contains a collection of tests for functional validation of the [Arduino Language Reference API](https://www.arduino.cc/reference/en/) and the main built-in libraries provided by Arduino cores (such as `Wire`, `SPI`, `CAN`, etc.).

> [!NOTE]
> These tests are primarily targeting cores which are implemented based on the [ArduinoCore-API](https://github.com/arduino/ArduinoCore-API). Nevertheless, most cores should be integrable with minimal modifications. 

## Intended Audience

This repository may be useful if:

- You actively develop and maintain Arduino cores
- You want to effectively validate the official Arduino Language Reference API with minimal effort on your Arduino-compatible target hardware
- You want to enable consistent and automated Hardware-in-the-Loop (HIL) testing for existing or new Arduino cores

## Test Overview

The following tests are available in this repository:

| [Language Reference API](https://www.arduino.cc/reference/en/) | Available Tests |
|------------------------ |-----------------|
| Digital I/O             | [digitalio](tests/digitalio) |
| Analog I/O              | [analogio](tests/analogio) |
| Analog ADC              | [analogio/adc](tests/analogio/adc) |
| External Interrupts     | [interrupts](tests/interrupts) |
| Tone                    | |
| Pulse                   | |
| Time                    | |
| **[CAN](https://github.com/arduino/ArduinoCore-API/blob/master/api/HardwareCAN.h)**| [can](tests/can) |
| CAN | [can/single](tests/can/single) | 
| SPI                     | |
| Wire                    | |
| Serial                  | |
| **Wi-Fi**               | [wifi](tests/wifi) |
| [WiFi Network](https://docs.arduino.cc/language-reference/en/functions/wifi/wificlass)  | [wifi/ap](tests/wifi/ap) - [wifi/sta](tests/wifi/sta)|
| | [wifi/extras](tests/wifi/extras) |
| | [wifi/exceptions](tests/wifi/exceptions) |
| [WiFiServer](https://docs.arduino.cc/language-reference/en/functions/wifi/server) - [WiFiClient](https://docs.arduino.cc/language-reference/en/functions/wifi/client) | [wifi/server](tests/wifi/server) - [wifi/client](tests/wifi/client) |
| [WiFiUDP](https://docs.arduino.cc/language-reference/en/functions/wifi/udp) | [wifi/udp_server](tests/wifi/udp_server) - [wifi/udp_client](tests/wifi/udp_client) |


## Tests Implementation Details

These tests are implemented using the [arduino-ottu-target](https://github.com/infineon/arduino-ottu-target) library, which provides [Unity](https://github.com/ThrowTheSwitch/Unity) test framework integration for Arduino-based projects.

Unity enables a structured and test-case-driven approach to validate the functionality of Arduino cores on actual hardware platforms.

Learn more about arduino-ottu-target [here](https://github.com/infineon/arduino-ottu-target).

The tests are designed for automated HIL testing in CI, as well as local testing during Arduino core development.

The [ottu](https://github.com/infineon/ottu) on-target testing utility supports both workflows by coordinating test execution, collecting results, and managing test sequencing and reporting.

Learn more about ottu [here](https://ottu.readthedocs.io/en/latest/).

## Requirements

To run these tests, the following prerequisites must be met:

- An Arduino-compatible hardware platform
- Arduino IDE or `arduino-cli` installed on the host machine
- [arduino-ottu-target](https://github.com/infineon/arduino-ottu-target) installed
- [ottu](https://github.com/infineon/ottu) on-target testing utility for orchestrating HIL tests (required for multi-target testing or advanced usage)

## How to Run the Tests

#### 1. Install all required assets

It is assumed that Arduino IDE or `arduino-cli` is already installed together with the required Arduino Core for your target hardware platform.

- Clone (or download) this repository to your local machine:

     ```bash
     git clone https://github.com/infineon/arduino-core-tests.git
     ```

- Install [arduino-ottu-target](https://github.com/infineon/arduino-ottu-target) via the Library Manager in Arduino IDE or using `arduino-cli`. The library name is `ottu-target`:

     ```bash
     arduino-cli lib install ottu-target
     ```

- If you plan to run multi-target tests, make sure to also install [`ottu`](https://github.com/infineon/ottu) on your host machine:

     ```bash
     pip install ottu
     ```

   > [!NOTE]
   > `ottu` is a CLI tool that supports and requires only **`arduino-cli`** (not the Arduino IDE). 

 
#### 2. Set up the devices under test (DUT)

Some tests require specific hardware peripherals or connections on the target test board (e.g. connected pins, pull-up resistors, or external modules). 

Make sure that your hardware setup meets these requirements before running the tests. **This information is available in each test's header comments.**

Tests requiring hardware setup include a `config.h` file where the necessary hardware configuration parameters, such as pin assignments and peripheral settings, are defined. 

You can use the suggested configuration if your board is already supported, or extend it to match your specific hardware setup.

Alternatively, in `arduino-cli`, these macros can also be defined via the `-D` compiler flag during the compilation process, allowing you to override or specify hardware configuration parameters without modifying the `config.h` file directly.

Example usage with `arduino-cli`:

    ```bash
    arduino-cli compile --fqbn <board_fqbn> --build-property "compiler.cpp.extra_flags=-DTEST_PIN_INPUT=9" <test_directory> 
    ```

#### 3. Compile and upload tests

Compile and upload the tests to the target hardware platform using the Arduino IDE or `arduino-cli`, after selecting the appropriate board. 

In `arduino-cli`:

     ```bash
     arduino-cli compile --fqbn <board_fqbn> <test_directory>
     arduino-cli upload -p <port> --fqbn <board_fqbn> <test_directory>
     ```

#### 4. Run multi-target tests

Some tests require multiple sketches to be executed on
different hardware platforms or boards simultaneously. 

You can coordinate execution manually with `arduino-cli` or the Arduino IDE, but this can become cumbersome and error-prone as the number of targets increases.

If you intend to perform multi-target testing efficiently and reliably, it is recommended to use [`ottu`](https://github.com/infineon/ottu), which automates the coordination and execution of tests across multiple hardware platforms, ensuring proper sequencing and result collection.


## Contributing

When contributing to this repository, please follow the [Contributing Guidelines](./CONTRIBUTING.md) and our [Code of Conduct](./CODE_OF_CONDUCT.md).


### General Design Guidelines

1. **Balance simplicity and coverage**.

   These tests are mainly intended to validate functional correctness of the primary and most common use cases. Therefore, most tests consist of "happy-path" scenarios, and only certain edge cases are covered.

   In embedded on-target testing, it can become challenging to implement certain
   corner cases and error scenarios. That can lead to increased complexity
   of the hardware and test code. 

   While there is no strict rule, ask yourself (and the code owners) whether generating a rare edge case or a "rainy day" scenario to increase the coverage justifies the cost in terms of added maintainability, complexity, and impact on usability.


2. Strive for adherence to the ArduinoCore-API standardization.

   The primary target and scope of these tests is to validate the [ArduinoCore-API](https://github.com/arduino/ArduinoCore-API) public API and its expected behavior. Thus, these tests are mainly intended for cores which are implemented based on the ArduinoCore-API standardization.

   In practice, even when a core is implemented in terms of the [ArduinoCore-API](https://github.com/arduino/ArduinoCore-API), it is possible that some of the public API functions are not implemented or behave differently than expected.

   If so, consider reviewing the Arduino core implementation for compliance. These tests can help improve consistency across cores.
   
   In other situations, each core might require a slightly different test behavior.
   Use conditional-compilation directives when a test needs board- or core-specific behavior.

   Challenge yourself to find the right balance between uniformity and core-specific behavior, and try to avoid unnecessary complexity in the tests.

   Ideally, these tests can be used by any Arduino core in the future, and that will be only possible if there is a high degree of commonality and reusability. 

3. Be aware that these are on-target black-box tests.
   
   While we are using Unity as a test framework, these tests are not the typical host unit tests.

   Unlike typical host-based unit tests, which are generally white-box tests aimed at achieving high line and code-path coverage, these are black-box tests that validate the public API and its expected behavior.

   Attempting to cover internal code paths is not the goal for these tests, and in edge cases, it will probably compromise simplicity as discussed in the first guideline.
   
   Formal coverage could be achieved by enabling the compiler coverage flags and running the tests on a host machine, but that is a run-time and test-setup overhead not considered necessary in this context.

   Additionally, in usual host-based unit tests, setup and teardown will ensure that each test is executed in a clean and isolated environment. 
   That is not something which can always easily be achieved in embedded on-target testing, primarily due to limitations on dynamic resource acquisition and release.

   That said, you will find many test cases are implemented in a sequential way with temporal dependency. The order of execution is important, and the tests are not always isolated.
   And, if enforcing isolation would substantially undermine principles 1 and 2, it may be better to avoid it.

### Adding New Tests

- Each test corresponds to a specific Arduino Language Reference API function or library. 

- When adding new tests, please follow the existing structure and naming conventions, and ensure that the test is well-documented with clear instructions for any required hardware setup.

- Include any core, board, or specific setup configurations in the `config.h` file or via compiler flags as appropriate.

- Guard any code which is specific to a given core or board with conditional compilation blocks (C-macros) to ensure compatibility across different Arduino cores.


### Enabling new Arduino Cores

- Add any required configuration for the intended core or board to each test’s `config.h`, so it provides a suggested or validated hardware setup.

## Arduino Cores Using These Tests

The following Arduino cores run these tests on their CI-based HIL setups:

- [XMC-for-Arduino](https://github.com/Infineon/XMC-for-Arduino)
- [arduino-core-psoc6](https://github.com/Infineon/arduino-core-psoc6)
- [ArduinoCore-zephyr (Infineon fork)](https://github.com/Infineon/ArduinoCore-zephyr)


## License

See [LICENSE](./LICENSE) for details.