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
#include <WiFi.h>
#include <WiFiClient.h>
#include "ottu.hpp"

/** 
 * Add your SSID_NAME and SSID_PASS to the secrets.h file. 
 * Ensure that the secrets.h file is placed directly in this directory.
 */
#include "secrets.h"

OTTUNITY_TEST(wifi_connect_to_ap) {
    int result = WiFi.begin(SSID_NAME, SSID_PASS);
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, result);
}

/* This IP address variable is declared as global
because it will be obtained in client_connect_by_hostname, 
and used by check_host_by_name */
IPAddress host_ip;

OTTUNITY_TEST(client_connect_by_hostname) {
    WiFiClient client;
    char host_name[] = "google.com";
    uint16_t port = 80;
    
    TEST_ASSERT_FALSE(client.connected());
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_UNINITED, client.status());
    
    TEST_ASSERT_TRUE(client.connect(host_name, port));
    TEST_ASSERT_TRUE(client.connected());
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_CONNECTED, client.status());
   
    Serial.print("Remote IP: ");
    host_ip = client.remoteIP();
    Serial.print(host_ip.toString().c_str());
    TEST_ASSERT_EQUAL_UINT16(port, client.remotePort());
    
    client.stop();
}

OTTUNITY_TEST(check_host_by_name) {
    IPAddress ip; 
    IPAddress ip_expected = host_ip;
    /* This domain provides a fix IP for testing */
    const char *hostname = "google.com";


    int ret = WiFi.hostByName(hostname, ip);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_STRING(ip_expected.toString().c_str(), ip.toString().c_str());
}

OTTUNITY_TEST(check_dns) {
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

OTTUNITY_TEST(check_ping) {
    int ret = WiFi.ping("github.com");
    TEST_ASSERT_EQUAL_INT(WIFI_ERROR_NONE, ret);
    ret = WiFi.ping(WiFi.gatewayIP());
    TEST_ASSERT_EQUAL_INT(WIFI_ERROR_NONE, ret);
}

OTTUNITY_TEST(check_set_dns){
    IPAddress dns1(8, 8, 8, 8);
    IPAddress dns2(8, 8, 4, 4);
    WiFi.setDNS(dns1);
    Serial.print(WiFi.dnsIP(0).toString().c_str());
    TEST_ASSERT_EQUAL_STRING(dns1.toString().c_str(), WiFi.dnsIP(0).toString().c_str());
    WiFi.setDNS(dns1, dns2);
    TEST_ASSERT_EQUAL_STRING(dns1.toString().c_str(), WiFi.dnsIP(0).toString().c_str());
    TEST_ASSERT_EQUAL_STRING(dns2.toString().c_str(), WiFi.dnsIP(1).toString().c_str());
}

OTTUNITY_TEST(wifi_disconnect) {
    WiFi.disconnect();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_STA_DISCONNECTED, WiFi.status());
}

OTTUNITY_TEST(wifi_end) {
    WiFi.end();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_UNINITED, WiFi.status());
}

OTTUNITY_RUN();