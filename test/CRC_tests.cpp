#include <gtest/gtest.h>
#include <PadBus/CRC.h>
#include <string>
#include <vector>

using namespace std;
using namespace padbus;

const string crc_data = "This is some test data to be converted to a CRC";
constexpr uint16_t valid_crc = 0x30fa;

TEST (CRC_tests, Test_CRC_Generation_Success) {
    const vector<uint8_t> buffer(crc_data.begin(), crc_data.end());
    const auto crc = CRC();
    EXPECT_EQ(valid_crc, crc.calculate(buffer));
}

TEST (CRC_tests, Test_CRC_Validation_Success) {
    vector<uint8_t> buffer(crc_data.begin(), crc_data.end());
    buffer.push_back(valid_crc >> 8);
    buffer.push_back(valid_crc & 0xff);
    const auto crc = CRC();
    uint16_t testResult = 0;
    for (const auto b : buffer) {
        uint8_t byte = 0;
        for (uint8_t i = 0 ; i < 8 ; i++) {
            const bool bit = b & (1 << 7-i);
            testResult = crc.calculateSerial(testResult, bit);
        }
    }

    EXPECT_EQ(0, testResult);
}

TEST (CRC_tests, Invalid_CRC_Validation_Failure) {
    vector<uint8_t> buffer(crc_data.begin(), crc_data.end());
    buffer.push_back(valid_crc >> 8);
    buffer.push_back(valid_crc & 0xff);
    buffer[0] = 0; // Replace first character in buffer so that CRC will no longer match.
    const auto crc = CRC();
    uint16_t testResult = 0;
    for (const auto b : buffer) {
        uint8_t byte = 0;
        for (uint8_t i = 0 ; i < 8 ; i++) {
            const bool bit = b & (1 << 7-i);
            testResult = crc.calculateSerial(testResult, bit);
        }
    }

    EXPECT_LT(0, testResult);
}
