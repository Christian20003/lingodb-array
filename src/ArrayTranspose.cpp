#include "../include/Array.h"

using lingodb::runtime::Array;

lingodb::runtime::VarLen32 Array::transpose() {
    if (!isNumericType()) {
        throw std::runtime_error("Array-Tranypose: This function is only supported with numeric element types");
    }
    if (!isSymmetric()) {
        throw std::runtime_error("Array-Transpose: This function allows only symmetric arrays");
    }

    // One dimensional arrays will increase in their dimension, otherwise it remains
    auto dimension = this->numberDimensions == 1 ? 2 : this->numberDimensions;
    // Get adjusted total metadata length
    auto metadataLength = 0;
    if (this->numberDimensions == 1) {
        metadataLength = 1 + this->metadata[1];
    } else if (this->numberDimensions == 2) {
        metadataLength = getMetadataLength() - getMetadataLength(2) + this->metadata[4];
    } else {
        metadataLength = getMetadataLength() - getMetadataLength(2) + this->metadata[5];
    }

    // Create result string in set its size
    std::string result;
    auto size = getStringSize(dimension, this->numberElements, metadataLength, getNullBytes(this->metadata[1]), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, &dimension, 1);
    writeToBuffer(buffer, &this->numberElements, 1);
    writeToBuffer(buffer, this->metadataLengths, this->numberDimensions);

    uint32_t dimLen1 = 0;
    uint32_t dimLen2 = 0;
    // If one dimensional it will get further metadata entries, because it will change to two dimensional
    if (this->numberDimensions == 1) {
        writeToBuffer(buffer, &this->metadata[1], 1);
        dimLen1 = this->metadata[1];
    // If two dimensional, override the metadata length with the elem-length of sub-entries
    } else if (this->numberDimensions == 2) {
        result.data()[3*sizeof(uint32_t)] = this->metadata[4];
        dimLen1 = this->metadata[4];
    // If multi-dimensional, override the metadata length with the dim-length of sub-entries
    } else {
        result.data()[3*sizeof(uint32_t)] = this->metadata[5];
        dimLen1 = this->metadata[5];
        dimLen2 = this->metadata[2];
    }

    // Write metadata entry of first dimension (only dim-length changes)
    writeToBuffer(buffer, this->metadata, 2);
    writeToBuffer(buffer, &dimLen1, 1);
    uint32_t offset = 0;
    uint32_t elemLength = this->metadata[1] / dimLen1;
    // Write metadata entry of second dimension (everything changes)
    for (size_t i = 0; i < dimLen1; i++) {
        writeToBuffer(buffer, &offset, 1);
        writeToBuffer(buffer, &elemLength, 1);
        writeToBuffer(buffer, &dimLen2, 1);
        offset += elemLength;
    }
    // Write the remaining metadata entries (they do not change)
    if (this->numberDimensions > 2) {
        auto idx = this->metadataLengths[0] + this->metadataLengths[1];
        writeToBuffer(buffer, &this->metadata[idx*3], (getMetadataLength() - idx)*3);
    }

    // If one-dimensional, copy all elements and nulls
    if (this->numberDimensions == 1) {
        copyElements(buffer);
        copyNulls(buffer, this->nulls, this->metadata[1], 0);
    // If multi-dimensional, re-adjust element order
    } else {
        // Number columns
        uint32_t cols = this->metadata[5] == 0 ? this->metadata[4] : this->metadata[5];
        // Number of elements in a row
        uint32_t rowSize = this->metadata[4];
        // Number of elements in a element 
        // (with more than 2 dimensions an element is a complete subarray)
        uint32_t elementSize = this->metadata[5] == 0 ? 1 : this->metadata[4] / this->metadata[5];
        std::vector<bool> nulls;

        // Iterate over each column
        for (size_t column = 0; column < cols; column++) {
            // Iterate over each row
            for (size_t row = 0; row < this->metadata[1]; row += rowSize) {
                // Identify the value range that must be copied
                uint32_t start = row + column * elementSize;
                uint32_t stop = start + elementSize - 1;
                // Iterate over each single element
                for (size_t k = start; k <= stop; k++) {
                    if (isNull(k)) {
                        nulls.push_back(true);  
                    } else {
                        copyElement(buffer, getElementPosition(k));
                        nulls.push_back(false);
                    } 
                }
            }
        }
        copyNulls(buffer, nulls);
    }
    return VarLen32::fromString(result);
}