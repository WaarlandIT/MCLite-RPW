#include <unity.h>
#include <Arduino.h>
#include "config/radio_presets.h"

using namespace mclite;

void test_apply_eu() {
    RadioConfig r;
    TEST_ASSERT_TRUE(applyRadioPreset(r, 0));   // eu_narrow
    TEST_ASSERT_EQUAL_UINT8(8, r.spreadingFactor);
    TEST_ASSERT_EQUAL_UINT8(8, r.codingRate);
    TEST_ASSERT_EQUAL_INT8(22, r.txPower);
    TEST_ASSERT_TRUE(r.frequency > 869.6f && r.frequency < 869.7f);
}

void test_apply_us() {
    RadioConfig r;
    TEST_ASSERT_TRUE(applyRadioPreset(r, 1));   // us_ca
    TEST_ASSERT_EQUAL_UINT8(7, r.spreadingFactor);
    TEST_ASSERT_EQUAL_UINT8(5, r.codingRate);
    TEST_ASSERT_TRUE(r.frequency > 910.5f && r.frequency < 910.6f);
}

void test_apply_out_of_range() {
    RadioConfig r;
    TEST_ASSERT_FALSE(applyRadioPreset(r, RADIO_PRESET_COUNT));
}

void test_match_defaults_eu() {
    RadioConfig r;   // defaults are the EU preset
    TEST_ASSERT_EQUAL_INT(0, matchRadioPreset(r));
}

void test_match_us() {
    RadioConfig r;
    applyRadioPreset(r, 1);
    TEST_ASSERT_EQUAL_INT(1, matchRadioPreset(r));
}

void test_match_custom() {
    RadioConfig r;
    r.spreadingFactor = 11;   // no preset uses SF11
    TEST_ASSERT_EQUAL_INT(-1, matchRadioPreset(r));
}

void test_match_ignores_tx_power() {
    RadioConfig r;
    applyRadioPreset(r, 1);
    r.txPower = 10;           // user-lowered TX must still match its region
    TEST_ASSERT_EQUAL_INT(1, matchRadioPreset(r));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_apply_eu);
    RUN_TEST(test_apply_us);
    RUN_TEST(test_apply_out_of_range);
    RUN_TEST(test_match_defaults_eu);
    RUN_TEST(test_match_us);
    RUN_TEST(test_match_custom);
    RUN_TEST(test_match_ignores_tx_power);
    return UNITY_END();
}
