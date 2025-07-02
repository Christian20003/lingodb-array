#include "../include/Array.h"

using lingodb::runtime::Array;

const uint32_t *Array::getFirstEntry(uint32_t dimension) {
    if (dimension > this->numberDimensions) {
        throw std::runtime_error("Requested dimension does not exist");
    }
    uint32_t *child = this->metadata;
    for (size_t i = 0; i < dimension - 1; i++) {
        child += this->metadataLengths[i] * 3;
    }
    return child;
}

const uint32_t *Array::getChildEntry(const uint32_t *entry, uint32_t dimension) {
    if (dimension > this->numberDimensions) {
        throw std::runtime_error("Requested array element does not exist");
    }
    if (entry[2] == 0) {
        return nullptr;
    }
    const uint32_t *child = getFirstEntry(dimension);
    uint32_t ignore = 0;
    uint32_t length = this->metadataLengths[dimension-1];
    for (size_t i = 0; i <= length * 3; i += 3) {
        if (entry == &child[i]) {
            return child += length * 3 + ignore * 3;
        } else {
            ignore += child[i+2];
        }
    }
    return nullptr;
}

uint32_t Array::getChildNumber(const uint32_t *element, uint32_t dimension) {
    if (dimension > this->numberDimensions) {
        throw std::runtime_error("Requested array element does not exist");
    }
    const uint32_t *start = getFirstEntry(dimension-1);
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