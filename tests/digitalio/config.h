#ifndef CONFIG_H
#define CONFIG_H

#if defined(CY8CKIT_062S2_AI)
#define TEST_PIN_DIGITAL_IO_OUTPUT 7
#define TEST_PIN_DIGITAL_IO_INPUT 6
#endif // CY8CKIT_062S2_AI

#if !defined(TEST_PIN_DIGITAL_IO_OUTPUT)
#error "TEST_PIN_DIGITAL_IO_OUTPUT is not defined, please define them before compiling"
#endif

#if !defined(TEST_PIN_DIGITAL_IO_INPUT)
#error "TEST_PIN_DIGITAL_IO_INPUT is not defined, please define them before compiling"
#endif

#endif // CONFIG_H