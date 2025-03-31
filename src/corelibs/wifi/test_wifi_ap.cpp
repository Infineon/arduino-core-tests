#include "test_common_includes.h"

#include <WiFi.h>

TEST_GROUP(wifi_ap);

static TEST_SETUP(wifi_ap) {
}

static TEST_TEAR_DOWN(wifi_ap) {
}

TEST_IFX(wifi_ap, config) {
    /* The settings of these values
    will be evaluated after beginAP()
    with the corresponding getter functions. */
    IPAddress local_ip(12, 34, 56, 78);
    IPAddress dns_server(8, 8, 8, 4);
    IPAddress gateway(12, 34, 56, 78);
    IPAddress subnet(255, 255, 128, 0);
    WiFi.config(local_ip, dns_server, gateway, subnet);
}

TEST_IFX(wifi_ap, begin_ap) {
    int result = WiFi.beginAP("arduino-wifi-ap", "wifi-ap-password", 1);
    TEST_ASSERT_EQUAL_INT(WL_AP_LISTENING, result);
}

TEST_IFX(wifi_ap, is_status_listening) {
    TEST_ASSERT_EQUAL_INT(WL_AP_LISTENING, WiFi.status());
}

TEST_IFX(wifi_ap, check_mac_address) {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC address: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(mac[i], HEX);
        if (i < 5) {
            Serial.print(":");
        }
    }
}

TEST_IFX(wifi_ap, check_local_ip) {
    IPAddress ip = WiFi.localIP();
    TEST_ASSERT_EQUAL_INT(12, ip[0]);
    TEST_ASSERT_EQUAL_INT(34, ip[1]);
    TEST_ASSERT_EQUAL_INT(56, ip[2]);
    TEST_ASSERT_EQUAL_INT(78, ip[3]);
}

TEST_IFX(wifi_ap, check_subnet_mask) {
    IPAddress ip = WiFi.subnetMask();
    TEST_ASSERT_EQUAL_INT(255, ip[0]);
    TEST_ASSERT_EQUAL_INT(255, ip[1]);
    TEST_ASSERT_EQUAL_INT(128, ip[2]);
    TEST_ASSERT_EQUAL_INT(0, ip[3]);
}

TEST_IFX(wifi_ap, check_gateway_ip) {
    IPAddress ip = WiFi.gatewayIP();
    TEST_ASSERT_EQUAL_INT(12, ip[0]);
    TEST_ASSERT_EQUAL_INT(34, ip[1]);
    TEST_ASSERT_EQUAL_INT(56, ip[2]);
    TEST_ASSERT_EQUAL_INT(78, ip[3]);    
}

TEST_IFX(wifi_ap, check_dns) {
    IPAddress dns_ip = WiFi.dnsIP(0);
    TEST_ASSERT_EQUAL_INT(8, dns_ip[0]);
    TEST_ASSERT_EQUAL_INT(8, dns_ip[1]);
    TEST_ASSERT_EQUAL_INT(8, dns_ip[2]);
    TEST_ASSERT_EQUAL_INT(4, dns_ip[3]);
}

TEST_IFX(wifi_ap, check_ssid) {
    const char *ssid = WiFi.SSID();
    TEST_ASSERT_EQUAL_STRING("arduino-wifi-ap", ssid);
}

TEST_IFX(wifi_ap, check_bssid) {
    uint8_t bssid[6];
    WiFi.BSSID(bssid);
    Serial.print("BSSID: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(bssid[i], HEX);
        if (i < 5) {
            Serial.print(":");
        }
    }
}

TEST_IFX(wifi_ap, check_encryption_type) {
    uint8_t encryption = WiFi.encryptionType();
    TEST_ASSERT_EQUAL_INT(AUTH_MODE_WPA2, encryption);
}

TEST_IFX(wifi_ap, disconnect) {
    WiFi.disconnect();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_AP_DISCONNECTED, WiFi.status());
}

TEST_IFX(wifi_ap, wifi_end) {
    WiFi.end();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_UNINITED, WiFi.status());
}

TEST_GROUP_RUNNER(wifi_ap) {
    RUN_TEST_CASE(wifi_ap, config);
    RUN_TEST_CASE(wifi_ap, begin_ap);
    RUN_TEST_CASE(wifi_ap, is_status_listening);
    RUN_TEST_CASE(wifi_ap, check_mac_address);
    RUN_TEST_CASE(wifi_ap, check_local_ip);
    RUN_TEST_CASE(wifi_ap, check_subnet_mask);
    RUN_TEST_CASE(wifi_ap, check_gateway_ip);
    RUN_TEST_CASE(wifi_ap, check_dns);
    RUN_TEST_CASE(wifi_ap, check_ssid);
    RUN_TEST_CASE(wifi_ap, check_bssid);
    RUN_TEST_CASE(wifi_ap, check_encryption_type);
    /* Wait forever for now. */
    /* This allows to check the sta
    test manually. */
    while(true) {};
    RUN_TEST_CASE(wifi_ap, disconnect);
    RUN_TEST_CASE(wifi_ap, wifi_end);
}


