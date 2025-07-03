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

bool Array::equalMetadata(const uint32_t *other) {
    auto size = getMetadataLength();
    // Iterate over each metadata value
    for (size_t i = 0; i < size * 3; i++) {
        if (this->metadata[i] != other[i]) {
            return false;
        }
    }
    return true;
}

bool Array::isSymmetric() {
    // Iterate over each dimension
    for (size_t i = 1; i <= this->numberDimensions; i++) {
        auto size = getMetadataLength(i);
        auto *start = getFirstEntry(i);
        uint32_t length = start[2];
        // Iterate over each metadata entry in a particular dimension
        for (size_t j = 0; j < size * 3; j += 3) {
            if (length != start[j+2]) {
                return false;
            }
        }
    }
    return true;
}