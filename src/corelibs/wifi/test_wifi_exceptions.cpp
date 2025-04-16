/**
 @brief This test runs basic tests that throws the exceptions.


 * @note The tests need to be run with an access point with internet access.
 */
#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiClient.h>

#include "secrets.h" // Add your SSID_NAME and SSID_PASS to the secrets.h file. Place the "secrets.h" file directly in the src/ directory.

TEST_GROUP(wifi_exceptions);

static TEST_SETUP(wifi_exceptions) {
}

static TEST_TEAR_DOWN(wifi_exceptions) {
}

TEST_IFX(wifi_exceptions, wifi_timeout) {
    WiFi.setTimeout(10); // 10ms timeout
    int result = WiFi.begin(SSID_NAME, SSID_PASS);
    //This should fail due to timeout
    TEST_ASSERT_EQUAL_INT(WIFI_ERROR_STA_CONNECT_FAILED, result);
    //Check if the connection is not established
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_STA_DISCONNECTED, WiFi.status());
}

TEST_GROUP_RUNNER(wifi_exceptions) {
    RUN_TEST_CASE(wifi_exceptions, wifi_timeout);
}


