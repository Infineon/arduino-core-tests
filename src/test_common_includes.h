#ifndef TEST_COMMON_INCLUDES_H
#define TEST_COMMON_INCLUDES_H

// std includes
#include <stdbool.h>

// project c includes

// test includes

// Unity c includes
#include "unity.h"
#include "unity_fixture.h"

// IFX Unity addons
#include "unity_ifx.h"

// Arduino includes
#include <Arduino.h>
#if defined(TEST_CAN_SINGLE) || defined(TEST_CAN_CONNECTED2_NODE1) || defined(TEST_CAN_CONNECTED2_NODE2)
    #include <CAN.h>
#endif

#if defined(TEST_WIRE_CONNECTED1_PINGPONG) || defined(TEST_WIRE_CONNECTED2_MASTERPINGPONG) || \
    defined(TEST_WIRE_CONNECTED2_SLAVEPINGPONG)
    #include <Wire.h>
#endif

// project cpp includes
#include "Utilities.hpp"

#endif // TEST_COMMON_INCLUDES_H
