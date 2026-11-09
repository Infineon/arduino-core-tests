#ifndef CONFIG_H
#define CONFIG_H

#if defined(CY8CKIT_062S2_AI)
#define TEST_PIN_DIGITAL_IO_OUTPUT 7
#define TEST_PIN_DIGITAL_IO_INPUT 6
#elif defined(CY8CPROTO_063_BLE)
#define TEST_PIN_DIGITAL_IO_OUTPUT 15
#define TEST_PIN_DIGITAL_IO_INPUT 14
#endif

#if !defined(TEST_PIN_DIGITAL_IO_OUTPUT)
#error "TEST_PIN_DIGITAL_IO_OUTPUT is not defined, please define them before compiling"
#endif

#if !defined(TEST_PIN_DIGITAL_IO_INPUT)
#error "TEST_PIN_DIGITAL_IO_INPUT is not defined, please define them before compiling"
#endif

#endif // CONFIG_H