#include "../include/Array.h"

bool Array::checkNull(int32_t position) {
    int32_t element = std::floor(position / 8);
    int32_t index = position % 8;
    int32_t shift = 8 - index - 1;
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

void Array::appendStringValue(std::string &target, uint32_t position) {
    auto *stringLengths = reinterpret_cast<uint32_t*>(this->elements);
    size_t prevLength = 0;

    for (size_t i = 0; i < (size_t) position; i++) {
        prevLength += stringLengths[i];
    }
    std::string value = std::string(this->strings + prevLength, stringLengths[position]);
    target.append("\"");
    target.append(value);
    target.append("\"");
}