#ifndef PADBUS_CRC_H
#define PADBUS_CRC_H
#include <cstdint>
#include <vector>

using namespace std;

namespace padbus {
    class CRC {
        const uint16_t POLYNOMIAL = 0x1021; // CRC-16-XMODEM
        uint16_t _lookupTable[256] {};

        public:
            CRC();
            [[nodiscard]] uint16_t calculateSerial(uint16_t currentCrc, bool bit) const;
            [[nodiscard]] uint16_t calculate(const vector<uint8_t> &data) const;
    };
}

#endif
