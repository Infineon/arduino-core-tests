#include "ottu.hpp"
#include <CAN.h>

#include "config.hpp"
#include "ottu.hpp"
#include <errno.h>
#include <string.h>

static const struct device *const canDevice =
    DEVICE_DT_GET(DT_PHANDLE_BY_IDX(DT_PATH(zephyr_user), cans, 0));

struct CallbackFrame {
    uint32_t id;
    uint8_t length;
    uint8_t data[64];
    void *context;
};

static struct k_msgq callbackFrames;
alignas(4) static char callbackStorage[sizeof(CallbackFrame) * 4];
static uint32_t callbackContext = 0xC0FFEE;

// Loopback mode lets this single controller receive its own transmissions.
static void enableCanLoopback() {
    TEST_ASSERT_EQUAL_INT(0, can_stop(canDevice));
    TEST_ASSERT_EQUAL_INT(0, can_set_mode(canDevice, CAN_MODE_LOOPBACK));
    TEST_ASSERT_EQUAL_INT(0, can_start(canDevice));
}

static void assertFrameRoundTrip(const CanMsg &sent) {
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(
        0, CAN.available(), "Receive queue must be empty before sending");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, CAN.write(sent),
                                  "CAN transmission failed");

    // Bound the wait so a missing loopback frame fails instead of hanging.
    const unsigned long startedAt = millis();
    while (CAN.available() == 0 && millis() - startedAt < TEST_CAN_TIMEOUT_MS) {
        delay(1);
    }

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, CAN.available(),
                                     "Expected one received loopback frame");
    const CanMsg received = CAN.read();
    TEST_ASSERT_EQUAL_HEX32(sent.id, received.id);
    TEST_ASSERT_EQUAL_UINT8(sent.data_length, received.data_length);
    if (sent.data_length > 0) {
        TEST_ASSERT_EQUAL_UINT8_ARRAY(sent.data, received.data,
                                      sent.data_length);
    }
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, CAN.available(),
                                     "Reading must consume the received frame");
}

/** Setup and tests for CAN initialization. The tests verify that the CAN
 * controller can begin at various bitrates and restart correctly after ending.
 */
OTTUNITY_GROUP(can_initialization);

OTTUNITY_GROUP_TEST_TEAR_DOWN(can_initialization) { CAN.end(); }

OTTUNITY_GROUP_TEST(can_initialization, begins_at_125k) {
    TEST_ASSERT_TRUE(CAN.begin(CanBitRate::BR_125k));
}

OTTUNITY_GROUP_TEST(can_initialization, begins_at_250k) {
    TEST_ASSERT_TRUE(CAN.begin(CanBitRate::BR_250k));
}

OTTUNITY_GROUP_TEST(can_initialization, begins_at_500k) {
    TEST_ASSERT_TRUE(CAN.begin(CanBitRate::BR_500k));
}

OTTUNITY_GROUP_TEST(can_initialization, begins_at_1M) {
    TEST_ASSERT_TRUE(CAN.begin(CanBitRate::BR_1000k));
}

OTTUNITY_GROUP_TEST(can_initialization, restarts_after_end) {
    TEST_ASSERT_TRUE(CAN.begin(CanBitRate::BR_125k));
    CAN.end();
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
}

/** Setup and tests for CAN loopback mode. The tests verify that standard and
 * extended frames, as well as various payload lengths and ID boundaries, are
 * correctly looped back.
 */
OTTUNITY_GROUP(can_loopback);

OTTUNITY_GROUP_TEST_SETUP(can_loopback) {
    TEST_ASSERT_TRUE_MESSAGE(CAN.begin(TEST_CAN_BITRATE),
                             "CAN initialization failed");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, can_stop(canDevice),
                                  "Cannot stop CAN controller");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, can_set_mode(canDevice, CAN_MODE_LOOPBACK),
                                  "CAN loopback is not supported");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, can_start(canDevice),
                                  "Cannot start CAN loopback");
}

OTTUNITY_GROUP_TEST_TEAR_DOWN(can_loopback) {
    // Discard leftovers, then restore normal mode for the next test group.
    while (CAN.available() > 0) {
        (void)CAN.read();
    }
    CAN.end();
    (void)can_set_mode(canDevice, CAN_MODE_NORMAL);
}

OTTUNITY_GROUP_TEST(can_loopback, preserves_standard_frame) {
    const uint8_t payload[] = {0x00, 0x11, 0x22, 0x33, 0xAA, 0xBB, 0xCC, 0xFF};
    const CanMsg sent(CanStandardId(TEST_CAN_ID), sizeof(payload), payload);

    assertFrameRoundTrip(sent);
}

OTTUNITY_GROUP_TEST(can_loopback, preserves_extended_frame) {
    const uint8_t payload[] = {0xFF, 0xCC, 0xBB, 0xAA, 0x33, 0x22, 0x11, 0x00};
    const CanMsg sent(CanExtendedId(0x01ABCDE3), sizeof(payload), payload);

    assertFrameRoundTrip(sent);
}

OTTUNITY_GROUP_TEST(can_loopback, preserves_standard_payload_lengths_0_to_8) {
    const uint8_t payload[] = {0x00, 0x11, 0x22, 0x33, 0xAA, 0xBB, 0xCC, 0xFF};
    for (uint8_t length = 0; length <= sizeof(payload); ++length) {
        const CanMsg sent(CanStandardId(TEST_CAN_ID), length, payload);
        assertFrameRoundTrip(sent);
    }
}

OTTUNITY_GROUP_TEST(can_loopback, preserves_extended_payload_lengths_0_to_8) {
    const uint8_t payload[] = {0xFF, 0xCC, 0xBB, 0xAA, 0x33, 0x22, 0x11, 0x00};
    for (uint8_t length = 0; length <= sizeof(payload); ++length) {
        const CanMsg sent(CanExtendedId(0x01ABCDE3), length, payload);
        assertFrameRoundTrip(sent);
    }
}

OTTUNITY_GROUP_TEST(can_loopback, preserves_standard_id_boundaries) {
    const uint8_t payload[] = {0x5A, 0xA5};
    const uint32_t ids[] = {0, 0x7FF};
    for (const uint32_t id : ids) {
        assertFrameRoundTrip(
            CanMsg(CanStandardId(id), sizeof(payload), payload));
    }
}

OTTUNITY_GROUP_TEST(can_loopback, preserves_extended_id_boundaries) {
    const uint8_t payload[] = {0x5A, 0xA5};
    const uint32_t ids[] = {0, 0x1FFFFFFF};
    for (const uint32_t id : ids) {
        assertFrameRoundTrip(
            CanMsg(CanExtendedId(id), sizeof(payload), payload));
    }
}

OTTUNITY_GROUP_TEST(can_loopback, preserves_payload_patterns) {
    const uint8_t patterns[] = {0x00, 0xFF, 0xAA, 0x55};
    for (const uint8_t pattern : patterns) {
        uint8_t payload[8];
        memset(payload, pattern, sizeof(payload));
        assertFrameRoundTrip(
            CanMsg(CanStandardId(TEST_CAN_ID), sizeof(payload), payload));
        assertFrameRoundTrip(
            CanMsg(CanExtendedId(0x01ABCDE3), sizeof(payload), payload));
    }
}

OTTUNITY_GROUP_TEST(can_loopback, preserves_mixed_frame_formats_and_lengths) {
    for (uint8_t sequence = 0; sequence < 18; ++sequence) {
        const uint8_t payload[] = {sequence, 0x11, 0x22, 0x33,
                                   0xAA,     0xBB, 0xCC, 0xFF};
        const uint32_t id = sequence % 2 == 0 ? CanStandardId(TEST_CAN_ID)
                                              : CanExtendedId(0x01ABCDE3);
        assertFrameRoundTrip(CanMsg(id, sequence % 9, payload));
    }
}

OTTUNITY_GROUP_TEST(can_loopback,
                    queues_frames_without_loss_duplication_or_reordering) {
    constexpr uint8_t frameCount = 8;
    TEST_ASSERT_EQUAL_UINT32(0, CAN.available());
    for (uint8_t sequence = 0; sequence < frameCount; ++sequence) {
        const uint8_t payload[] = {sequence,
                                   static_cast<uint8_t>(sequence ^ 0xFF)};
        TEST_ASSERT_EQUAL_INT(1, CAN.write(CanMsg(CanStandardId(TEST_CAN_ID),
                                                  sizeof(payload), payload)));
    }

    const unsigned long startedAt = millis();
    while (CAN.available() < frameCount &&
           millis() - startedAt < TEST_CAN_TIMEOUT_MS) {
        delay(1);
    }
    TEST_ASSERT_EQUAL_UINT32(frameCount, CAN.available());
    for (uint8_t sequence = 0; sequence < frameCount; ++sequence) {
        const CanMsg received = CAN.read();
        TEST_ASSERT_EQUAL_HEX32(CanStandardId(TEST_CAN_ID), received.id);
        TEST_ASSERT_EQUAL_UINT8(2, received.data_length);
        TEST_ASSERT_EQUAL_UINT8(sequence, received.data[0]);
        TEST_ASSERT_EQUAL_UINT8(sequence ^ 0xFF, received.data[1]);
        TEST_ASSERT_EQUAL_UINT32(frameCount - sequence - 1, CAN.available());
    }
    const unsigned long drainedAt = millis();
    while (millis() - drainedAt < TEST_CAN_QUIET_MS) {
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, CAN.available(),
                                         "Unexpected duplicate frame");
        delay(1);
    }
}

OTTUNITY_GROUP_TEST(can_loopback,
                    sustained_mixed_traffic_preserves_sequence_and_payload) {
    for (uint32_t sequence = 0; sequence < TEST_CAN_STRESS_FRAME_COUNT;
         ++sequence) {
        uint8_t payload[8];
        payload[0] = static_cast<uint8_t>(sequence);
        payload[1] = static_cast<uint8_t>(sequence >> 8);
        for (uint8_t index = 2; index < sizeof(payload); ++index) {
            payload[index] = static_cast<uint8_t>(sequence ^ (index * 0x35));
        }
        const uint32_t id = sequence % 2 == 0
                                ? CanStandardId(sequence & 0x7FF)
                                : CanExtendedId(0x01000000 | sequence);
        assertFrameRoundTrip(CanMsg(id, 2 + sequence % 7, payload));
    }
    const unsigned long drainedAt = millis();
    while (millis() - drainedAt < TEST_CAN_QUIET_MS) {
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, CAN.available(),
                                         "Unexpected late stress frame");
        delay(1);
    }
}

OTTUNITY_GROUP_TEST(can_loopback, repeated_restarts_at_alternating_bitrates) {
    const CanBitRate bitrates[] = {CanBitRate::BR_125k, CanBitRate::BR_250k,
                                   CanBitRate::BR_500k, CanBitRate::BR_1000k};
    CAN.end();
    for (unsigned int cycle = 0; cycle < TEST_CAN_LIFECYCLE_CYCLES; ++cycle) {
        TEST_ASSERT_TRUE(CAN.begin(bitrates[cycle % 4]));
        enableCanLoopback();
        const uint8_t payload[] = {static_cast<uint8_t>(cycle),
                                   static_cast<uint8_t>(cycle >> 8), 0xAA,
                                   0x55};
        assertFrameRoundTrip(
            CanMsg(CanStandardId(TEST_CAN_ID), sizeof(payload), payload));
        assertFrameRoundTrip(
            CanMsg(CanExtendedId(0x01ABCDE3), sizeof(payload), payload));
        CAN.end();
    }
}

static void assertFrameRejected(const CanMsg &sent) {
    TEST_ASSERT_EQUAL_UINT32(0, CAN.available());
    TEST_ASSERT_EQUAL_INT(1, CAN.write(sent));
    // Keep checking through the quiet interval to catch delayed reception.
    const unsigned long startedAt = millis();
    while (millis() - startedAt < TEST_CAN_QUIET_MS) {
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(
            0, CAN.available(), "Receive filter accepted an unexpected frame");
        delay(1);
    }
}

/** Setup and tests for CAN receive filters. The tests verify that standard and
 * extended filters correctly accept or reject frames based on exact match and
 * mask criteria.
 */
OTTUNITY_GROUP(can_filters);

OTTUNITY_GROUP_TEST_SETUP(can_filters) {
    // Start with exact-match filters for one standard and one extended ID.
    TEST_ASSERT_EQUAL_INT(1, CAN.addReceiveFilter(0x123, 0x7FF));
    TEST_ASSERT_EQUAL_INT(1,
                          CAN.addReceiveFilter(0x01ABCDE3, 0x1FFFFFFF, true));
}

OTTUNITY_GROUP_TEST_TEAR_DOWN(can_filters) {
    CAN.end();
    while (CAN.available() > 0) {
        (void)CAN.read();
    }
    (void)can_set_mode(canDevice, CAN_MODE_NORMAL);
    TEST_ASSERT_EQUAL_INT(1, CAN.addReceiveFilter(0, 0));
    TEST_ASSERT_EQUAL_INT(1, CAN.addReceiveFilter(0, 0, true));
}

OTTUNITY_GROUP_TEST(can_filters, standard_exact_match_and_format_separation) {
    const uint8_t payload[] = {0x5A, 0xA5};
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
    enableCanLoopback();
    assertFrameRoundTrip(
        CanMsg(CanStandardId(0x123), sizeof(payload), payload));
    assertFrameRejected(CanMsg(CanStandardId(0x124), sizeof(payload), payload));
    assertFrameRejected(CanMsg(CanExtendedId(0x123), sizeof(payload), payload));
}

OTTUNITY_GROUP_TEST(can_filters, extended_exact_match_and_format_separation) {
    const uint8_t payload[] = {0x5A, 0xA5};
    TEST_ASSERT_EQUAL_INT(1, CAN.addReceiveFilter(0x123, 0x1FFFFFFF, true));
    TEST_ASSERT_EQUAL_INT(1, CAN.addReceiveFilter(0x456, 0x7FF));
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
    enableCanLoopback();
    assertFrameRoundTrip(
        CanMsg(CanExtendedId(0x123), sizeof(payload), payload));
    assertFrameRejected(CanMsg(CanExtendedId(0x124), sizeof(payload), payload));
    assertFrameRejected(CanMsg(CanStandardId(0x123), sizeof(payload), payload));
}

OTTUNITY_GROUP_TEST(can_filters, standard_mask_accepts_only_matching_range) {
    const uint8_t payload[] = {0x12};
    TEST_ASSERT_EQUAL_INT(1, CAN.addReceiveFilter(0x120, 0x7F0));
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
    enableCanLoopback();
    assertFrameRoundTrip(
        CanMsg(CanStandardId(0x120), sizeof(payload), payload));
    assertFrameRoundTrip(
        CanMsg(CanStandardId(0x12F), sizeof(payload), payload));
    assertFrameRejected(CanMsg(CanStandardId(0x130), sizeof(payload), payload));
}

OTTUNITY_GROUP_TEST(can_filters, extended_mask_accepts_only_matching_range) {
    const uint8_t payload[] = {0x34};
    TEST_ASSERT_EQUAL_INT(1,
                          CAN.addReceiveFilter(0x01ABCDE0, 0x1FFFFFF0, true));
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
    enableCanLoopback();
    assertFrameRoundTrip(
        CanMsg(CanExtendedId(0x01ABCDE0), sizeof(payload), payload));
    assertFrameRoundTrip(
        CanMsg(CanExtendedId(0x01ABCDEF), sizeof(payload), payload));
    assertFrameRejected(
        CanMsg(CanExtendedId(0x01ABCDF0), sizeof(payload), payload));
}

OTTUNITY_GROUP_TEST(can_filters, replaces_standard_filter_after_restart) {
    const uint8_t payload[] = {0x12, 0x34};
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
    enableCanLoopback();
    assertFrameRoundTrip(
        CanMsg(CanStandardId(0x123), sizeof(payload), payload));
    CAN.end();

    TEST_ASSERT_EQUAL_INT(1, CAN.addReceiveFilter(0x456, 0x7FF));
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
    enableCanLoopback();
    assertFrameRoundTrip(
        CanMsg(CanStandardId(0x456), sizeof(payload), payload));
    assertFrameRejected(CanMsg(CanStandardId(0x123), sizeof(payload), payload));
    assertFrameRoundTrip(
        CanMsg(CanExtendedId(0x01ABCDE3), sizeof(payload), payload));
}

OTTUNITY_GROUP_TEST(can_filters, replaces_extended_filter_after_restart) {
    const uint8_t payload[] = {0x56, 0x78};
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
    enableCanLoopback();
    assertFrameRoundTrip(
        CanMsg(CanExtendedId(0x01ABCDE3), sizeof(payload), payload));
    CAN.end();

    TEST_ASSERT_EQUAL_INT(1,
                          CAN.addReceiveFilter(0x01234567, 0x1FFFFFFF, true));
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
    enableCanLoopback();
    assertFrameRoundTrip(
        CanMsg(CanExtendedId(0x01234567), sizeof(payload), payload));
    assertFrameRejected(
        CanMsg(CanExtendedId(0x01ABCDE3), sizeof(payload), payload));
    assertFrameRoundTrip(
        CanMsg(CanStandardId(0x123), sizeof(payload), payload));
}

OTTUNITY_GROUP_TEST(can_filters,
                    rejects_changes_while_running_without_changing_filters) {
    const uint8_t payload[] = {0xA5};
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
    enableCanLoopback();
    TEST_ASSERT_LESS_THAN_INT(0, CAN.addReceiveFilter(0x456, 0x7FF));
    TEST_ASSERT_LESS_THAN_INT(
        0, CAN.addReceiveFilter(0x01234567, 0x1FFFFFFF, true));
    assertFrameRoundTrip(
        CanMsg(CanStandardId(0x123), sizeof(payload), payload));
    assertFrameRoundTrip(
        CanMsg(CanExtendedId(0x01ABCDE3), sizeof(payload), payload));
    assertFrameRejected(CanMsg(CanStandardId(0x456), sizeof(payload), payload));
    assertFrameRejected(
        CanMsg(CanExtendedId(0x01234567), sizeof(payload), payload));
}

static void captureCanFrame(const CanFDMsg &message, void *userData) {
    // Callback delivery uses its own queue so polling reads stay independent.
    CallbackFrame captured = {};
    captured.id = message.id;
    captured.length = message.data_length;
    memcpy(captured.data, message.data, message.data_length);
    captured.context = userData;
    (void)k_msgq_put(&callbackFrames, &captured, K_NO_WAIT);
}

static void assertCallbackRoundTrip(const CanMsg &sent) {
    TEST_ASSERT_EQUAL_INT(1, CAN.write(sent));
    CallbackFrame received = {};
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0, k_msgq_get(&callbackFrames, &received, K_MSEC(TEST_CAN_TIMEOUT_MS)),
        "Timed out waiting for CAN receive callback");
    TEST_ASSERT_EQUAL_PTR(&callbackContext, received.context);
    TEST_ASSERT_EQUAL_HEX32(sent.id, received.id);
    TEST_ASSERT_EQUAL_UINT8(sent.data_length, received.length);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(sent.data, received.data, sent.data_length);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(
        0, CAN.available(), "Callback frames must not enter the polling queue");
}

OTTUNITY_GROUP(can_callbacks);

OTTUNITY_GROUP_TEST_SETUP(can_callbacks) {
    k_msgq_init(&callbackFrames, callbackStorage, sizeof(CallbackFrame), 4);
    TEST_ASSERT_TRUE(CAN.begin(TEST_CAN_BITRATE));
    enableCanLoopback();
    CAN.onReceive(captureCanFrame, &callbackContext);
}

OTTUNITY_GROUP_TEST_TEAR_DOWN(can_callbacks) {
    CAN.clearReceiveCallback();
    CAN.end();
    while (CAN.available() > 0) {
        (void)CAN.read();
    }
    (void)can_set_mode(canDevice, CAN_MODE_NORMAL);
}

OTTUNITY_GROUP_TEST(can_callbacks,
                    delivers_standard_and_extended_frames_with_context) {
    const uint8_t payload[] = {0x00, 0xFF, 0xAA, 0x55};
    assertCallbackRoundTrip(
        CanMsg(CanStandardId(TEST_CAN_ID), sizeof(payload), payload));
    assertCallbackRoundTrip(
        CanMsg(CanExtendedId(0x01ABCDE3), sizeof(payload), payload));
    CallbackFrame unexpected = {};
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        -EAGAIN,
        k_msgq_get(&callbackFrames, &unexpected, K_MSEC(TEST_CAN_QUIET_MS)),
        "Unexpected duplicate callback");
}

OTTUNITY_GROUP_TEST(can_callbacks, clearing_callback_restores_polling) {
    const uint8_t payload[] = {0x12, 0x34, 0x56, 0x78};
    const CanMsg sent(CanStandardId(TEST_CAN_ID), sizeof(payload), payload);
    assertCallbackRoundTrip(sent);
    CAN.clearReceiveCallback();
    assertFrameRoundTrip(sent);
    CallbackFrame unexpected = {};
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        -EAGAIN,
        k_msgq_get(&callbackFrames, &unexpected, K_MSEC(TEST_CAN_QUIET_MS)),
        "Callback was still invoked after clearing it");
}

OTTUNITY_GROUP_TEST(can_callbacks,
                    repeatedly_switches_between_callback_and_polling) {
    for (unsigned int cycle = 0; cycle < TEST_CAN_CALLBACK_TRANSITIONS;
         ++cycle) {
        uint8_t payload[] = {static_cast<uint8_t>(cycle),
                             static_cast<uint8_t>(cycle >> 8), 0xC0, 0xAA};
        const uint32_t id = cycle % 2 == 0 ? CanStandardId(TEST_CAN_ID)
                                           : CanExtendedId(0x01ABCDE3);
        CAN.onReceive(captureCanFrame, &callbackContext);
        assertCallbackRoundTrip(CanMsg(id, sizeof(payload), payload));

        CAN.clearReceiveCallback();
        payload[2] = 0x50;
        assertFrameRoundTrip(CanMsg(id, sizeof(payload), payload));
        CallbackFrame unexpected = {};
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            -EAGAIN,
            k_msgq_get(&callbackFrames, &unexpected, K_MSEC(TEST_CAN_QUIET_MS)),
            "Stale or unexpected callback during polling phase");
    }
}

OTTUNITY_RUN();