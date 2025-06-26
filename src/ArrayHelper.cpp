#include "../include/Array.h"

using lingodb::runtime::Array;

void Array::copyElements(char *&buffer) {
    if (type == mlir::Type::INTEGER) {
        writeToBuffer(buffer, reinterpret_cast<int32_t*>(this->elements), this->numberElements);
    } else if (type == mlir::Type::BIGINTEGER) {
        writeToBuffer(buffer, reinterpret_cast<int64_t*>(this->elements), this->numberElements);
    } else if (type == mlir::Type::FLOAT) {
        writeToBuffer(buffer, reinterpret_cast<float*>(this->elements), this->numberElements);
    } else if (type == mlir::Type::DOUBLE) {
        writeToBuffer(buffer, reinterpret_cast<double*>(this->elements), this->numberElements);
    } else if (type == mlir::Type::STRING) {
        writeToBuffer(buffer, reinterpret_cast<uint32_t*>(this->elements), this->numberElements);
    } else {
        throw std::runtime_error("Given type is not supported in arrays");
    }
}

void Array::copyStrings(char *&buffer) {
    if (type != mlir::Type::STRING) {
        return;
    }
    size_t size = getTotalStringLength();
    writeToBuffer(buffer, this->strings, size);
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

const uint32_t *Array::getFirstElement(uint32_t dimension) {
    if (dimension > this->numberDimensions) {
        throw std::runtime_error("Requested dimension does not exist");
    }
    uint32_t *child = this->metadata;
    for (size_t i = 0; i < dimension; i++) {
        child += this->metadataLengths[i] * 3;
    }
    return child;
}

const uint32_t *Array::getFirstChild(uint32_t dimension, uint32_t childNumber) {
    if (dimension > this->numberDimensions) {
        throw std::runtime_error("Requested array element does not exist");
    }
    if (this->metadataLengths[dimension - 1] < childNumber) {
        throw std::runtime_error("Requested array element does not exist");
    }
    const uint32_t *child = getFirstElement(dimension-1);
    uint32_t ignore = 0;
    for (size_t i = 0; i <= childNumber * 3; i += 3) {
        if (i == childNumber * 3) {
            if (child[i+2] == 0) {
                return nullptr;
            }
        } else {
            ignore += child[i+2];
        }
    }
    child += this->metadataLengths[dimension-1] * 3 + ignore * 3;
    return child;
}

uint32_t Array::getChildNumber(const uint32_t *element, uint32_t dimension) {
    if (dimension > this->numberDimensions) {
        throw std::runtime_error("Requested array element does not exist");
    }
    const uint32_t *start = getFirstElement(dimension-1);
    uint32_t result = 0;
    for (size_t i = 0; i < this->metadataLengths[dimension - 1] * 3; i += 3) {
        if (start + i != element) {
            result++;
        } else {
            return result;
        }
    }
    return 0;
}

uint32_t Array::getElementPosition(uint32_t position) {
    return position - countNulls(position);
}