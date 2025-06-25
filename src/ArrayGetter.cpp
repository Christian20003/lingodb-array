#include "../include/Array.h"

size_t Array::getStringSize(uint32_t dimensions, uint32_t numberElements, uint32_t metadataSize, uint32_t nullSize, uint32_t stringSize, mlir::Type type) {
    size_t size = sizeof(uint32_t) * 2 + dimensions * sizeof(uint32_t) + metadataSize * 3 * sizeof(uint32_t);
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
    size += std::ceil((double) nullSize / 8);
    size += stringSize;
    return size;
}

size_t Array::getTypeSize(mlir::Type type) {
    if (type == mlir::Type::INTEGER) {
        return sizeof(int32_t);
    } else if (type == mlir::Type::BIGINTEGER) {
        return sizeof(int64_t);
    } else if (type == mlir::Type::FLOAT) {
        return sizeof(float);
    } else if (type == mlir::Type::DOUBLE) {
        return sizeof(double);
    } else if (type == mlir::Type::STRING) {
        return sizeof(uint32_t);
    } else {
        throw std::runtime_error("Given type is not supported in arrays");
    }
}

size_t Array::getMetadataLength() {
    size_t result = 0;
    for (size_t i = 0; i < this->numberDimensions; i++) {
        result += this->metadataLengths[i];
    }
    return result;
}

size_t Array::getBrackets(uint32_t position) {
    size_t result = 0;
    for (size_t i = 0; i < getMetadataLength() * 2; i += 2) {
        if (this->metadata[i] == position) {
            result++;
        }
    }
    return result;
}

size_t Array::getEmptyElement(uint32_t position) {
    size_t metadataIdx = 0;
    for (size_t i = 0; i < this->numberDimensions; i++) {
        for (size_t j = 0; j < this->metadataLengths[i]; j++) {
            if (position == this->metadata[metadataIdx] && this->metadata[metadataIdx + 1] == 0) {
                return i+1;
            }
            metadataIdx += 2;
        }
    }
    return 0;
}