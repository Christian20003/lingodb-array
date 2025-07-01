#include "../include/Array.h"

using lingodb::runtime::Array;

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