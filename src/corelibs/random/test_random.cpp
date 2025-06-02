/**
 * @brief test_random.cpp
 *
 * @details This test script verifies the functionality of the `Random(max)` and `Random(min, max)` APIs on Board.
 */

// Includes
#include "test_common_includes.h"
#include "Arduino.h"

#define MAX_VALUE 20000
#define MIN_VALUE 10000 
#define NUM_SAMPLES 10000 

// A boolean array to track generated numbers
static bool number_tracker[MAX_VALUE];

/**
 * @brief Suite setup function, runs before test suite execution begins.
 */
static void random_suite_setup() {
}

/**
 * @brief Suite teardown function, runs after test suite execution is complete.
 */
static void random_suite_teardown() {
    
}

// Test Group
TEST_GROUP(random);

/**
 * @brief Setup function for Board1.
 */
TEST_SETUP(random) {

}

/**
 * @brief Teardown function for Board1.
 */
TEST_TEAR_DOWN(random) {
}

/**
 * @brief Test Case verifies random(max) generates unique numbers within the range [0, max-1]
 * 
 * @details The Frequency Distribution Test is a statistical method used to assess the randomness 
 *          and uniformity of a sequence of generated numbers. It checks whether the numbers are 
 *          evenly distributed across a defined range. This is particularly important for testing 
 *          random number generators (RNGs), as a good RNG should produce numbers that are uniformly 
 *          distributed across the entire rang
 */
TEST_IFX(random, test_random_max_frequency_distribution) {
    const int NUM_BINS = 10;        // Divide the range into 10 bins for frequency distribution
    int frequency[NUM_BINS] = {0};  // Array to count occurrences in each bin
    float expected_count = (float)NUM_SAMPLES / NUM_BINS;  // Expected count per bin
    float tolerance = 0.1 * expected_count;  // Allowable tolerance (10% of expected count)

    randomSeed(analogRead(0)); 

    for (int i = 0; i < NUM_SAMPLES; i++) {
        long rand_num = random(MAX_VALUE);

        // Validate that rand_num is within the expected range
        TEST_ASSERT_LESS_THAN(MAX_VALUE, rand_num);
        TEST_ASSERT_GREATER_OR_EQUAL(0, rand_num);

        // Map the random number to a bin for frequency counting
        int bin = (rand_num * NUM_BINS) / MAX_VALUE; 
        frequency[bin]++;
    }

    // Evaluate the frequency distribution across bins
    for (int i = 0; i < NUM_BINS; i++) {
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(
            tolerance,
            expected_count,
            frequency[i],
            "Frequency distribution in bin is outside acceptable range"
        );
    }
}


/**
 * @brief Test Case verifies random(max) generates unique numbers within the range [min, max-1]
 * 
 * @details The Frequency Distribution Test is a statistical method used to assess the randomness 
 *          and uniformity of a sequence of generated numbers. It checks whether the numbers are 
 *          evenly distributed across a defined range. This is particularly important for testing 
 *          random number generators (RNGs), as a good RNG should produce numbers that are uniformly 
 *          distributed across the entire rang
 */
TEST_IFX(random, test_random_min_max_frequency_distribution) {
    const int NUM_BINS = 10;  // Divide the range into 10 bins for frequency distribution
    int frequency[NUM_BINS] = {0};  // Array to count occurrences in each bin
    float expected_count = (float)NUM_SAMPLES / NUM_BINS;  // Expected count per bin
    float tolerance = 0.1 * expected_count;  // Allowable tolerance (10% of expected count)

    randomSeed(analogRead(0)); 

    for (int i = 0; i < NUM_SAMPLES; i++) {
        long rand_num = random(MIN_VALUE, MAX_VALUE);

        // Validate that the random number is within the expected range
        TEST_ASSERT_LESS_THAN(MAX_VALUE, rand_num);
        TEST_ASSERT_GREATER_OR_EQUAL(MIN_VALUE, rand_num);

        // Map the random number to a bin for frequency counting
        int bin = (rand_num - MIN_VALUE) * NUM_BINS / (MAX_VALUE - MIN_VALUE);
        frequency[bin]++;
    }

    // Evaluate the frequency distribution across bins
    for (int i = 0; i < NUM_BINS; i++) {
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(
            tolerance,
            expected_count,
            frequency[i],
            "Frequency distribution in bin is outside acceptable range"
        );
    }
}

/**
 * @brief Bundle all tests for shiftIn and shiftOut.
 */
TEST_GROUP_RUNNER(random) {
    random_suite_setup();

    RUN_TEST_CASE(random, test_random_max_frequency_distribution);
    RUN_TEST_CASE(random, test_random_min_max_frequency_distribution);

    random_suite_teardown();
}
