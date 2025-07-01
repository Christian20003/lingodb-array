#include "../include/Array.h"

using lingodb::runtime::Array;

void Array::castNulls(std::vector<bool> &nulls, char *&writer) {
    for (size_t i = 0; i < nulls.size(); i++) {
        if (i != 0 && i % 8 == 0) {
            writer += 1;
        }
        if (nulls[i]) {
            uint8_t index = i % 8;
            uint8_t shift = 8 - index - 1;
            *writer |= (1 << shift);
        }
    }
    writer += 1;
}

void Array::appendNulls(char *&buffer, const uint8_t *nulls, uint32_t numberElements, uint32_t startNumber) {
    for (size_t i = 0; i < numberElements; i++) {
        if (i != 0 && i % 8 == 0) {
            nulls++;
        }
        uint8_t index = i % 8;
        uint8_t shift = 8 - index - 1;
        bool isNull = nulls[0] >> shift;
        if (isNull) {
            uint8_t newIndex = startNumber % 8;
            uint8_t newShift = 8 - newIndex - 1;
            *buffer |= (1 << newShift);
        }
        startNumber++;
        if (startNumber % 8 == 0) {
            buffer++;
        }
    }
    buffer++;
}

bool Array::checkNull(uint32_t position) {
    uint32_t element = std::floor(position / 8);
    uint32_t index = position % 8;
    uint32_t shift = 8 - index - 1;
    return 1 & this->nulls[element] >> shift;
}

uint32_t Array::countNulls(uint32_t maxPosition) {
    uint32_t result = 0;
    for (size_t i = 0; i < maxPosition; i++) {
        if (checkNull(i)) {
            result++;
        }
    }
    return result;
}

uint32_t Array::countNullBytes(uint32_t numberElements) {
    return std::ceil((double) numberElements / 8);
}