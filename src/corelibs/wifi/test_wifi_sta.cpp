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
 * This test is paired with the "test_wifi_ap.cpp"s test, which
 * running on a second board will provide the access point to which 
 * the board running this tests connects to. 
 * 
 * @note This test must be run after the "test_wifi_ap.cpp" test. 
 */
#include "test_common_includes.h"

#include <WiFi.h>

TEST_GROUP(wifi_sta);

static TEST_SETUP(wifi_sta) {
}

static TEST_TEAR_DOWN(wifi_sta) {
}

/**
 * @brief Scan available WiFi networks and searches for the expected SSID
 *        of the access point created by the "test_wifi_ap.cpp" test.
 * 
 * @details Once the SSID is found, it also checks the BSSID, RSSI and encryption type.
 * 
 * @note The test rssi assert expects that the station and the access point are physically closed 
 *       to each other (as they would be in a hardware-in-the-loop setup)
 * 
 * @note If the SSID is not found, the subsequent test requiring a established
 *       connection will fail.
 */
TEST_IFX(wifi_sta, scan_networks) {
    int8_t num_networks = WiFi.scanNetworks();
    /* Look for the AP matching SSID */
    int8_t net_index = 0;
    for (; net_index < num_networks; net_index++) {
        const char *ssid = WiFi.SSID(net_index);
        if(strcmp(ssid, "arduino-wifi-ap") == 0) {
            break;
        }
    }

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

TEST_IFX(wifi_sta, config_ip_static) {
    /* The settings of these values
    will be evaluated after beginAP()
    with the corresponding getter functions. */
    IPAddress local_ip(12, 34, 56, 111);
    IPAddress dns_server(4, 4, 4, 4);
    IPAddress gateway(12, 34, 56, 78);
    IPAddress subnet(255, 255, 128, 0);
    WiFi.config(local_ip, dns_server, gateway, subnet);
}

TEST_IFX(wifi_sta, connect_to_ap) {
    /* This AP is created by the test_wifi_ap.
       Currently the test tools does NOT provide a way to and synch multitest. 
       It has only be validated manually. */
    int result = WiFi.begin("arduino-wifi-ap", "wifi-ap-password");
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, result);
}

TEST_IFX(wifi_sta, is_status_connected) {
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, WiFi.status());
}

TEST_IFX(wifi_sta, check_mac_address) {
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

TEST_IFX(wifi_sta, check_local_ip) {
    IPAddress ip = WiFi.localIP();
    TEST_ASSERT_EQUAL_INT(12, ip[0]);
    TEST_ASSERT_EQUAL_INT(34, ip[1]);
    TEST_ASSERT_EQUAL_INT(56, ip[2]);
    TEST_ASSERT_EQUAL_INT(111, ip[3]);
}

TEST_IFX(wifi_sta, check_subnet_mask) {
    IPAddress ip = WiFi.subnetMask();
    TEST_ASSERT_EQUAL_INT(255, ip[0]);
    TEST_ASSERT_EQUAL_INT(255, ip[1]);
    TEST_ASSERT_EQUAL_INT(128, ip[2]);
    TEST_ASSERT_EQUAL_INT(0, ip[3]);
}

TEST_IFX(wifi_sta, check_gateway_ip) {
    IPAddress ip = WiFi.gatewayIP();
    TEST_ASSERT_EQUAL_INT(12, ip[0]);
    TEST_ASSERT_EQUAL_INT(34, ip[1]);
    TEST_ASSERT_EQUAL_INT(56, ip[2]);
    TEST_ASSERT_EQUAL_INT(78, ip[3]);
}

TEST_IFX(wifi_sta, check_dns) {
    IPAddress dns_ip = WiFi.dnsIP(0);
    TEST_ASSERT_EQUAL_INT(4, dns_ip[0]);
    TEST_ASSERT_EQUAL_INT(4, dns_ip[1]);
    TEST_ASSERT_EQUAL_INT(4, dns_ip[2]);
    TEST_ASSERT_EQUAL_INT(4, dns_ip[3]);
}

TEST_IFX(wifi_sta, check_ssid) {
    const char *ssid = WiFi.SSID();
    TEST_ASSERT_EQUAL_STRING("arduino-wifi-ap", ssid);
}

TEST_IFX(wifi_sta, check_bssid) {
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
TEST_IFX(wifi_sta, check_rssi) {
    int32_t rssi = WiFi.RSSI();
    TEST_ASSERT_TRUE(rssi > INT32_MIN);
    TEST_ASSERT_TRUE(-50 < rssi && rssi < 50);
}

TEST_IFX(wifi_sta, check_encryption_type) {
    uint8_t encryption = WiFi.encryptionType();
    TEST_ASSERT_EQUAL_INT(AUTH_MODE_WPA2, encryption);
}

TEST_IFX(wifi_sta, disconnect) {
    WiFi.disconnect();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_STA_DISCONNECTED, WiFi.status());
}

TEST_IFX(wifi_sta, wifi_end) {
    WiFi.end();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_UNINITED, WiFi.status());
}

TEST_GROUP_RUNNER(wifi_sta) {
    RUN_TEST_CASE(wifi_sta, scan_networks);
    RUN_TEST_CASE(wifi_sta, config_ip_static);
    RUN_TEST_CASE(wifi_sta, connect_to_ap);
    RUN_TEST_CASE(wifi_sta, is_status_connected);
    RUN_TEST_CASE(wifi_sta, check_mac_address);
    RUN_TEST_CASE(wifi_sta, check_local_ip);
    RUN_TEST_CASE(wifi_sta, check_subnet_mask);
    RUN_TEST_CASE(wifi_sta, check_gateway_ip);
    RUN_TEST_CASE(wifi_sta, check_dns);
    RUN_TEST_CASE(wifi_sta, check_ssid);
    RUN_TEST_CASE(wifi_sta, check_bssid);
    RUN_TEST_CASE(wifi_sta, check_rssi);
    RUN_TEST_CASE(wifi_sta, check_encryption_type);
    RUN_TEST_CASE(wifi_sta, disconnect);
    RUN_TEST_CASE(wifi_sta, wifi_end);  
    while(true) {};
}


