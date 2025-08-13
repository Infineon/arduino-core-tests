/**
 * @brief This test runs some basic check on WiFi station API 
 * functions which require internet access.
 * 
 * @details The tested function are those require domain name handling like ping
 * or connection by domain name.
 * 
 * This test is meant as a happy path verification. Exception and corner/edge cases
 * are not covered in this test. 
 * 
 * @note The tests need to be run with an access point with internet access.
 */
#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiClient.h>

/** 
 * Add your SSID_NAME and SSID_PASS to the secrets.h file. 
 * Place the "secrets.h" file directly in the src/ directory.
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

TEST_IFX(wifi_extras, check_dns) {
    IPAddress dns_ip = WiFi.dnsIP(0);
    /* No assertion possible here, as the DNS
    address is not set upfront */
    Serial.print("DNS: ");
    for (int i = 0; i < 4; i++) {
        Serial.print(dns_ip[i], DEC);
        if (i < 3) {
            Serial.print(".");
        }
    }
}

TEST_IFX(wifi_extras, check_ping) {
    int ret = WiFi.ping("github.com");
    TEST_ASSERT_EQUAL_INT(WIFI_ERROR_NONE, ret);
    ret = WiFi.ping(WiFi.gatewayIP());
    TEST_ASSERT_EQUAL_INT(WIFI_ERROR_NONE, ret);
}

TEST_IFX(wifi_extras, check_set_dns){
    IPAddress dns1(8, 8, 8, 8);
    IPAddress dns2(8, 8, 4, 4);
    WiFi.setDNS(dns1);
    Serial.print(WiFi.dnsIP(0).toString().c_str());
    TEST_ASSERT_EQUAL_STRING(dns1.toString().c_str(), WiFi.dnsIP(0).toString().c_str());
    WiFi.setDNS(dns1, dns2);
    TEST_ASSERT_EQUAL_STRING(dns1.toString().c_str(), WiFi.dnsIP(0).toString().c_str());
    TEST_ASSERT_EQUAL_STRING(dns2.toString().c_str(), WiFi.dnsIP(1).toString().c_str());
}

TEST_IFX(wifi_extras, wifi_disconnect) {
    WiFi.disconnect();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_STA_DISCONNECTED, WiFi.status());
}

TEST_IFX(wifi_extras, wifi_end) {
    WiFi.end();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_UNINITED, WiFi.status());
}

TEST_GROUP_RUNNER(wifi_extras) {
    RUN_TEST_CASE(wifi_extras, wifi_connect_to_ap);
    RUN_TEST_CASE(wifi_extras, client_connect_by_hostname);
    RUN_TEST_CASE(wifi_extras, check_host_by_name);
    RUN_TEST_CASE(wifi_extras, check_dns);
    RUN_TEST_CASE(wifi_extras, check_ping);
    RUN_TEST_CASE(wifi_extras, check_set_dns);
    RUN_TEST_CASE(wifi_extras, wifi_disconnect);
    RUN_TEST_CASE(wifi_extras, wifi_end);    
}