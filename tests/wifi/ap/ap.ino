/**
 * @brief This test will set up a WiFi access point and set/get the basic settings.
 * 
 * @details This test is meant as a happy path verification of the API functions for 
 * Access Point mode. 
 * 
 * Exception and corner/edge cases are not covered in this test. 
 * 
 * This test is paired with the "sta.ino" test, which
 * running on a second board will connect to the AP created by this test.
 */

#include <WiFi.h>
#include "ottu.hpp"

OTTUNITY_TEST(config) {
    /* The settings of these values
    will be evaluated after beginAP()
    with the corresponding getter functions. */
    IPAddress local_ip(12, 34, 56, 78);
    IPAddress dns_server(8, 8, 8, 4);
    IPAddress gateway(12, 34, 56, 78);
    IPAddress subnet(255, 255, 128, 0);
    WiFi.config(local_ip, dns_server, gateway, subnet);
}

OTTUNITY_TEST(begin_ap) {
    int result = WiFi.beginAP("arduino-wifi-ap", "wifi-ap-password", 1);
    TEST_ASSERT_EQUAL_INT(WL_AP_LISTENING, result);
}

OTTUNITY_TEST(is_status_listening) {
    // wait for the station to connect
    while (WiFi.connected()==0) {}
    TEST_ASSERT_EQUAL_INT(WL_AP_LISTENING, WiFi.status());
}

OTTUNITY_TEST(check_mac_address) {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    /* No assertion possible here, as the
    device adddress is not known upfront */
    Serial.print("MAC address: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(mac[i], HEX);
        if (i < 5) {
            Serial.print(":");
        }
    }
}

OTTUNITY_TEST(check_local_ip) {
    IPAddress ip = WiFi.localIP();
    TEST_ASSERT_EQUAL_INT(12, ip[0]);
    TEST_ASSERT_EQUAL_INT(34, ip[1]);
    TEST_ASSERT_EQUAL_INT(56, ip[2]);
    TEST_ASSERT_EQUAL_INT(78, ip[3]);
}

OTTUNITY_TEST(check_subnet_mask) {
    IPAddress ip = WiFi.subnetMask();
    TEST_ASSERT_EQUAL_INT(255, ip[0]);
    TEST_ASSERT_EQUAL_INT(255, ip[1]);
    TEST_ASSERT_EQUAL_INT(128, ip[2]);
    TEST_ASSERT_EQUAL_INT(0, ip[3]);
}

OTTUNITY_TEST(check_gateway_ip) {
    IPAddress ip = WiFi.gatewayIP();
    TEST_ASSERT_EQUAL_INT(12, ip[0]);
    TEST_ASSERT_EQUAL_INT(34, ip[1]);
    TEST_ASSERT_EQUAL_INT(56, ip[2]);
    TEST_ASSERT_EQUAL_INT(78, ip[3]);    
}

OTTUNITY_TEST(check_dns) {
    IPAddress dns_ip = WiFi.dnsIP(0);
    TEST_ASSERT_EQUAL_INT(8, dns_ip[0]);
    TEST_ASSERT_EQUAL_INT(8, dns_ip[1]);
    TEST_ASSERT_EQUAL_INT(8, dns_ip[2]);
    TEST_ASSERT_EQUAL_INT(4, dns_ip[3]);
}

OTTUNITY_TEST(check_ssid) {
    const char *ssid = WiFi.SSID();
    TEST_ASSERT_EQUAL_STRING("arduino-wifi-ap", ssid);
}

OTTUNITY_TEST(check_bssid) {
    uint8_t bssid[6];
    WiFi.BSSID(bssid);
    /* No assertion possible here, as the
    device adddress is not known upfront */
    Serial.print("BSSID: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(bssid[i], HEX);
        if (i < 5) {
            Serial.print(":");
        }
    }
}

OTTUNITY_TEST(check_encryption_type) {
    uint8_t encryption = WiFi.encryptionType();
    TEST_ASSERT_EQUAL_INT(AUTH_MODE_WPA2, encryption);
}

OTTUNITY_TEST(disconnect) {
    // wait for the station to disconnect first
    while (WiFi.connected()!=0) {}

    WiFi.disconnect();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_AP_DISCONNECTED, WiFi.status());
}

OTTUNITY_TEST(wifi_end) {
    WiFi.end();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_UNINITED, WiFi.status());
}

OTTUNITY_RUN();

