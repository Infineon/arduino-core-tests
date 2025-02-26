#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiClient.h>

/** 
 * These tests need to be run with an access point with internet access
 * Therefore, it won´t be part of the automated tests.
 * Add your SSID_NAME and SSID_PASS to the secrets.h file.
*/
#include "secrets.h"

TEST_GROUP(wifi_extras);

static TEST_SETUP(wifi_extras) {
}

static TEST_TEAR_DOWN(wifi_extras) {
}

TEST_IFX(wifi_extras, wifi_connect_to_ap) {
    int result = WiFi.begin(SSID_NAME, SSID_PASS);
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, result);
}


TEST_IFX(wifi_extras, client_connect_by_hostname) {
    WiFiClient client;
    char host_name[] = "google.com";
    uint16_t port = 80;
    
    TEST_ASSERT_FALSE(client.connected());
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_UNINITED, client.status());
    
    TEST_ASSERT_TRUE(client.connect(host_name, port));
    TEST_ASSERT_TRUE(client.connected());
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_CONNECTED, client.status());
   
    Serial.print("Remote IP: ");
    Serial.print(client.remoteIP().toString().c_str());
    TEST_ASSERT_EQUAL_UINT16(port, client.remotePort());
    
    client.stop();
}

TEST_IFX(wifi_extras, check_host_by_name) {
    IPAddress ip; 
    IPAddress ip_expected = IPAddress(127, 0, 0, 1);
    /* This domain provides a fix IP for testing */
    const char *hostname = "localhost";

    int ret = WiFi.hostByName(hostname, ip);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_STRING(ip_expected.toString().c_str(), ip.toString().c_str());
}

TEST_IFX(wifi_extras, wifi_end) {
    WiFi.end();
}

TEST_GROUP_RUNNER(wifi_extras) {
    RUN_TEST_CASE(wifi_extras, wifi_connect_to_ap);
    RUN_TEST_CASE(wifi_extras, client_connect_by_hostname);
    RUN_TEST_CASE(wifi_extras, check_host_by_name);
    RUN_TEST_CASE(wifi_extras, wifi_end);    
    /** 
     * TODO: Remove when this is added to cicd 
     * and more tests are added to the suite together with the 
     * WiFi ones.
     * For now it is more controllable like this to run tests 
     * manually. 
     */
    while(true) {};
}