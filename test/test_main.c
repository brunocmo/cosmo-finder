#include <unity.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void testTrueIsTrue() {
    uint8_t wow;
    wow = 1;
    TEST_ASSERT_TRUE( wow );
}

int app_main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST( testTrueIsTrue );

    UNITY_END();

    return 0;
}
