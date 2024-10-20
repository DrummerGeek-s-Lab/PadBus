#include <PadBus/CRC.h>

using namespace std;
using namespace padbus;

CRC::CRC() {
    // Prepare the lookup table
    for (uint16_t i = 0; i < 256; i++) {
        uint16_t crc = i << 8;  // Shift the byte to the most significant 8 bits
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ POLYNOMIAL;  // Apply the polynomial
            } else {
                crc <<= 1;
            }
        }

        _lookupTable[i] = crc;  // Store the CRC value for the byte
    }
}

uint16_t CRC::calculateSerial(const uint16_t currentCrc, const bool bit) const {
    uint16_t result = currentCrc << 1 | bit;

    if (currentCrc & 0x8000) {
        result ^= POLYNOMIAL;
    }

    return result;
}

uint16_t CRC::calculate(const vector<uint8_t> &data) const {
    uint16_t crc = 0x0000;  // Initial value for CRC-16 XMODEM
    for (auto& byte : data) {
        const uint8_t lookupIndex = (crc >> 8) ^ byte;  // XOR high byte of CRC with the input byte
        crc = (crc << 8) ^ _lookupTable[lookupIndex];    // Lookup table value and update CRC
    }
    return crc;
}
