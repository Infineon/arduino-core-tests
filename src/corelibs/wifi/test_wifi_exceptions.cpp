/**
 @brief This test runs basic tests that throws the exceptions.
 *
 */
#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiClient.h>

/** 
 * Add your SSID_NAME and SSID_PASS to the secrets.h file. 
 * Ensure that the secrets.h file is placed directly in the ./test directory of the project root.
 */
#include "secrets.h" 

TEST_GROUP(wifi_exceptions);

static TEST_SETUP(wifi_exceptions) {
}

static TEST_TEAR_DOWN(wifi_exceptions) {
}

/// Test for WiFi.begin() timeout
/// @note This test should fail due to timeout. so make the WiFi not available
TEST_IFX(wifi_exceptions, wifi_timeout) {
    // Set a short timeout for the connection attempt
    WiFi.setTimeout(10); // 10 ms timeout
    int result = WiFi.begin(SSID_NAME, SSID_PASS);
    //This should fail due to timeout
    TEST_ASSERT_EQUAL_INT(WIFI_ERROR_STA_CONNECT_FAILED, result);
    //Check if the connection is not established
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_STA_DISCONNECTED, WiFi.status());
}

/// Test for WiFi.reasonCode()
TEST_IFX(wifi_exceptions, wifi_reasoncode){    
    uint8_t reason_code = WiFi.reasonCode();
    TEST_ASSERT_EQUAL_UINT8(WIFI_ERROR_STA_CONNECT_FAILED, reason_code);
    reason_code = WiFi.ping("google.com");
    TEST_ASSERT_EQUAL_UINT8(WIFI_ERROR_PING_FAILED, reason_code);
}

TEST_GROUP_RUNNER(wifi_exceptions) {
    RUN_TEST_CASE(wifi_exceptions, wifi_timeout);
    RUN_TEST_CASE(wifi_exceptions, wifi_reasoncode);
}