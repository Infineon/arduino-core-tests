#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <CAN.h>

#if defined(ARDUINO_KIT_PSC3M5_EVK)
static const struct device *const canDevice =
    DEVICE_DT_GET(DT_PHANDLE_BY_IDX(DT_PATH(zephyr_user), cans, 0));
#endif // ARDUINO_KIT_PSC3M5_EVK

constexpr CanBitRate TEST_CAN_BITRATE = CanBitRate::BR_500k;
constexpr uint32_t TEST_CAN_ID = 0x123;
constexpr unsigned long TEST_CAN_TIMEOUT_MS = 1000;
constexpr unsigned long TEST_CAN_QUIET_MS = 50;
constexpr uint32_t TEST_CAN_STRESS_FRAME_COUNT = 2048;
constexpr unsigned int TEST_CAN_LIFECYCLE_CYCLES = 20;
constexpr unsigned int TEST_CAN_CALLBACK_TRANSITIONS = 20;

static_assert(TEST_CAN_STRESS_FRAME_COUNT > 0 &&
                  TEST_CAN_STRESS_FRAME_COUNT <= 65536,
              "Stress frame count must fit the two-byte sequence number");
static_assert(TEST_CAN_LIFECYCLE_CYCLES > 0 &&
                  TEST_CAN_CALLBACK_TRANSITIONS > 0,
              "Reliability checks must run at least one cycle");

#endif // CONFIG_H