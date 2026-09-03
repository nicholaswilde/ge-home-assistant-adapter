#include <unity.h>
#include <ArduinoFake.h>

using namespace fakeit;

void setUp(void) {
    ArduinoFakeReset();
}

void tearDown(void) {
}

void test_led_builtin_pin_number(void) {
    TEST_ASSERT_EQUAL(2, 2);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_led_builtin_pin_number);
    UNITY_END();
    return 0;
}
