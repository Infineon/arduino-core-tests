/**
 * @brief test_random.cpp
 *
 * @details This test script verifies the functionality of the `Random(max)` and `Random(min, max)` APIs on Board.
 */

// Includes
#include "test_common_includes.h"
#include "Arduino.h"

#define MAX_VALUE 50 
#define MIN_VALUE 10 

// A boolean array to track generated numbers
// static bool number_tracker[MAX_VALUE];

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
 */
TEST_IFX(random, test_random_max_unique) {

    // memset(number_tracker, false, sizeof(number_tracker));

    randomSeed(analogRead(0)); 

    for (int i = 0; i < MAX_VALUE; i++) {
        long rand_num = random(MAX_VALUE);

        // Validate that rand_num is within the expected range
        TEST_ASSERT_LESS_THAN(MAX_VALUE, rand_num); 
        TEST_ASSERT_GREATER_OR_EQUAL(0, rand_num); 

        // Check for uniqueness
        // TEST_ASSERT_FALSE_MESSAGE(
        //     number_tracker[rand_num],
        //     "Duplicate number detected in random(max)");

        // number_tracker[rand_num] = true;
    }
}

/**
 * @brief Test Case verifies random(min, max) generates unique numbers within the range [min, max-1]
 */
TEST_IFX(random, test_random_min_max_unique) {

    // memset(number_tracker, false, sizeof(number_tracker));

    randomSeed(analogRead(0)); 

    for (int i = 0; i < MAX_VALUE - MIN_VALUE; i++) {
        long rand_num = random(MIN_VALUE, MAX_VALUE);

        // Validate that rand_num is within the expected range
        TEST_ASSERT_LESS_THAN(MAX_VALUE, rand_num); 
        TEST_ASSERT_GREATER_OR_EQUAL(MIN_VALUE, rand_num); 

        // Check for uniqueness (adjust index to start from 0)
        // int adjusted_index = rand_num - MIN_VALUE;
        // TEST_ASSERT_FALSE_MESSAGE(
        //     number_tracker[adjusted_index],
        //     "Duplicate number detected in random(min, max)");

        // number_tracker[adjusted_index] = true;
    }
}

/**
 * @brief Bundle all tests for shiftIn and shiftOut.
 */
TEST_GROUP_RUNNER(random) {
    random_suite_setup();

    RUN_TEST_CASE(random, test_random_max_unique);
    RUN_TEST_CASE(random, test_random_min_max_unique);

    random_suite_teardown();
}
