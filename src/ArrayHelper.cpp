#include "../include/Array.h"

size_t Array::calculateSize(uint32_t dimensions, uint32_t numberElements, uint32_t metadataSize, uint32_t nullSize, uint32_t stringSize, mlir::Type type) {
    size_t size = sizeof(uint32_t) * 2 + dimensions * sizeof(uint32_t) + metadataSize * 2 * sizeof(uint32_t);
    if (type == mlir::Type::INTEGER) {
        size += numberElements * sizeof(int32_t);
    } else if (type == mlir::Type::BIGINTEGER || type == mlir::Type::STRING) {
        size += numberElements * sizeof(int64_t);
    } else if (type == mlir::Type::FLOAT) {
        size += numberElements * sizeof(float);
    } else if (type == mlir::Type::DOUBLE) {
        size += numberElements * sizeof(double);
    } else {
        throw std::runtime_error("Given type is not supported in arrays");
    }
    size += std::ceil(nullSize / 8);
    size += stringSize;
    return size;
}

bool Array::checkNull(int32_t position) {
    int32_t element = std::floor(position / 8);
    int32_t index = position % 8;
    int32_t shift = 8 - index - 1;
    return this->nulls[element] >> shift;
}

int32_t Array::countNulls() {
    int32_t result = 0;
    for (size_t i = 0; i < this->numberElements; i++) {
        if (checkNull(i)) {
            result++;
        }
    }
    return result;
}