#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiClient.h>

TEST_GROUP(wifi_client);

static TEST_SETUP(wifi_client) {
}

static TEST_TEAR_DOWN(wifi_client) {
}

TEST_IFX(wifi_client, begin_ap) {
}

TEST_IFX(wifi_client, wifi_end) {
}

TEST_GROUP_RUNNER(wifi_client) {
    RUN_TEST_CASE(wifi_client, begin_ap);
    RUN_TEST_CASE(wifi_client, wifi_end);
}