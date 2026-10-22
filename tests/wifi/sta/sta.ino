/**
 * @brief This test will establish a connection as a WiFi station with a WiFi 
 * access point.
 * 
 * @details It validates the basic settings for a connected WiFi station.
 * This test is meant as a happy path verification of the API functions for 
 * station mode. 
 * 
 * Exception and corner/edge cases are not covered in this test. 
 * 
 * This test is paired with the "ap.ino" test, which
 * running on a second board will provide the access point to which 
 * the board running this tests connects to. 
 * 
 * @note This test must be run after the "ap.ino" test. 
 */
#include <WiFi.h>
#include "ottu.hpp"

/**
 * @brief Scan available WiFi networks and searches for the expected SSID
 *        of the access point created by the "test_wifi_ap.cpp" test.
 * 
 * @details Once the SSID is found, it also checks the BSSID, RSSI and encryption type.
 * 
 * @note The test rssi assert expects that the station and the access point are physically closed 
 *       to each other (as they would be in a hardware-in-the-loop setup)
 * 
 * @note The scan is repeated for up to 10 seconds. If the SSID is not found
 *       by then, the subsequent test requiring a established connection will fail.
 */
OTTUNITY_TEST(scan_networks) {
    /* The AP board may still be starting, so rescan until it shows up. */
    const uint32_t scan_timeout_ms = 10000;
    const uint32_t start_ms = millis();
    int8_t net_index = -1;
    do {
        int8_t num_networks = WiFi.scanNetworks();
        for (int8_t i = 0; i < num_networks; i++) {
            const char *ssid = WiFi.SSID(i);
            if (strcmp(ssid, "arduino-wifi-ap") == 0) {
                net_index = i;
                break;
            }
        }
    } while (net_index < 0 && (millis() - start_ms) < scan_timeout_ms);

    TEST_ASSERT_TRUE_MESSAGE(net_index >= 0, "SSID arduino-wifi-ap not found within 10 s");
    TEST_ASSERT_EQUAL_STRING("arduino-wifi-ap", WiFi.SSID(net_index));

    /* Network bssid */
    uint8_t bssid[6];
    WiFi.BSSID(net_index, bssid);
    /* No assertion possible here, as the
    device adddress is not known upfront */
    Serial.print("BSSID: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(bssid[i], HEX);
        if (i < 5) {
            Serial.print(":");
        }
    }

    int32_t rssi = WiFi.RSSI(net_index);
    /* Assuming the boards are a few cm separate from each other. */
    TEST_ASSERT_TRUE(-50 < rssi && rssi < 50);

    TEST_ASSERT_EQUAL_INT(AUTH_MODE_WPA2, WiFi.encryptionType(net_index));
    TEST_ASSERT_EQUAL_INT(1, WiFi.channel(net_index));
}

OTTUNITY_TEST(config_ip_static) {
    /* The settings of these values
    will be evaluated after beginAP()
    with the corresponding getter functions. */
    IPAddress local_ip(12, 34, 56, 111);
    IPAddress dns_server(4, 4, 4, 4);
    IPAddress gateway(12, 34, 56, 78);
    IPAddress subnet(255, 255, 128, 0);
    WiFi.config(local_ip, dns_server, gateway, subnet);
}

OTTUNITY_TEST(connect_to_ap) {
    /* This AP is created by the test_wifi_ap.
       Currently the test tools does NOT provide a way to and synch multitest. 
       It has only be validated manually. */
    int result = WiFi.begin("arduino-wifi-ap", "wifi-ap-password");
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, result);
}

OTTUNITY_TEST(is_status_connected) {
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, WiFi.status());
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
    TEST_ASSERT_EQUAL_INT(111, ip[3]);
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
    TEST_ASSERT_EQUAL_INT(4, dns_ip[0]);
    TEST_ASSERT_EQUAL_INT(4, dns_ip[1]);
    TEST_ASSERT_EQUAL_INT(4, dns_ip[2]);
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

/**
 * @brief Check the rssi level of the access point.
 * 
 * @note The tests requires the access point board and the station to be a few cm apart.
 *       This is assumed to be the case in a hardware-in-the-loop setup. 
 */
OTTUNITY_TEST(check_rssi) {
    int32_t rssi = WiFi.RSSI();
    TEST_ASSERT_TRUE(rssi > INT32_MIN);
    TEST_ASSERT_TRUE(-50 < rssi && rssi < 50);
}

OTTUNITY_TEST(check_encryption_type) {
    uint8_t encryption = WiFi.encryptionType();
    TEST_ASSERT_EQUAL_INT(AUTH_MODE_WPA2, encryption);
}

OTTUNITY_TEST(disconnect) {
    WiFi.disconnect();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_STA_DISCONNECTED, WiFi.status());
}

OTTUNITY_TEST(wifi_end) {
    WiFi.end();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_UNINITED, WiFi.status());
}

OTTUNITY_RUN();

