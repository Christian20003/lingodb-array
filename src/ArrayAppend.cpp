#include "../include/Array.h"

using lingodb::runtime::Array;

lingodb::runtime::VarLen32 Array::append(Array &other) {
    if (this->type != other.getType()) {
        throw std::runtime_error("Array-Append: Arrays have different types");
    }
    if (this->numberDimensions < other.getDimension()) {
        throw std::runtime_error("Array-Append: Right array has more dimensions than left array");
    }

    uint32_t leftDimension = this->numberDimensions;
    uint32_t rightDimension = other.getDimension();

    uint32_t leftElements = this->numberElements;
    uint32_t leftTotalElements = this->metadata[1];
    uint32_t rightElements = other.getNumberElements();
    uint32_t rightTotalElements = other.getTotalNumberElements();

    uint32_t leftStringLength = getTotalStringLength();
    uint32_t rightStringLength = other.getTotalStringLength();

    // result will store the extended array
    std::string result = "";
    uint32_t newMetadataSize = getMetadataLength();
    if (this->numberDimensions == other.getDimension()) {
        newMetadataSize += other.getMetadataLength() - 1;
    } else {
        newMetadataSize += other.getMetadataLength();
    }
    // First step: calculate the new size of the resulting string
    size_t size = getStringSize(
        leftDimension, 
        leftElements + rightElements,
        newMetadataSize,
        countNullBytes(leftTotalElements + rightTotalElements),
        leftStringLength + rightStringLength,
        type
    );
    result.resize(size);
    char *buffer = result.data();

    // Add number dimensions and elements to the result
    uint32_t numberElements = leftElements + rightElements;
    writeToBuffer(buffer, &leftDimension, 1);
    writeToBuffer(buffer, &numberElements, 1);

    // Add metadata lengths to the result
    // The number of entries will be the same as on the left
    uint32_t leftIdx = 0;
    uint32_t rightIdx = 0;
    for (size_t i = leftDimension; i > 0; i--) {
        // Append right to next higher dimension on left (right will be new child)
        if (rightDimension == i && rightDimension < leftDimension) {
            uint32_t newLength = this->metadataLengths[leftIdx] + other.getMetadataLength(rightIdx);
            writeToBuffer(buffer, &newLength, 1);
            rightIdx++;
        // Left and right have same dimension. Append all childs from right to left
        } else if (rightDimension == i) {
            writeToBuffer(buffer, &this->metadataLengths[leftIdx], 1);
            rightIdx++;
        // Left has much more dimensions than right (Copy values from left)
        } else if (rightDimension < i) {
            writeToBuffer(buffer, &this->metadataLengths[leftIdx], 1);
        // Otherwise copy new child metadata length values
        } else {
            uint32_t newLength = this->metadataLengths[leftIdx] + other.getMetadataLength(rightIdx);
            writeToBuffer(buffer, &newLength, 1);
            rightIdx++;
        }
        leftIdx++;
    }

    // Add metadata entries to the result
    uint32_t leftMetadataIdx = 0;
    uint32_t rightMetadataIdx = 0;
    bool onlyAppend = false;
    for (size_t i = leftDimension; i > 0; i--) {
        const uint32_t *leftMetadata = getFirstElement(leftMetadataIdx);
        const uint32_t *rightMetadata = other.getFirstElement(rightMetadataIdx);
        uint32_t leftMetadataLength = getMetadataLength(leftMetadataIdx);
        uint32_t rightMetadataLength = other.getMetadataLength(rightMetadataIdx);
        // Check if dimension level is reached to append array structures from right
        if (rightDimension <= i && !onlyAppend) {
            // Iterate over each metadata entry from left in current dimension
            for (size_t j = 0; j < leftMetadataLength * 3; j += 3) {
                // Check if last entry is selected
                if (j + 3 == leftMetadataLength * 3) {
                    // Adjust its element length value
                    writeToBuffer(buffer, &leftMetadata[j], 1);
                    uint32_t newElemLength = leftMetadata[j+1] + rightMetadata[1];
                    writeToBuffer(buffer, &newElemLength, 1);
                    // If left has more dimensions than right, append rights entries as new childs to the 
                    // next higher dimension of left
                    if (rightDimension + 1 == i) {
                        uint32_t newDimLength = leftMetadata[j+2] + 1;
                        writeToBuffer(buffer, &newDimLength, 1);
                        onlyAppend = true;
                    // If in the same dimension level
                    } else if (rightDimension == i) {
                        uint32_t newDimLength = leftMetadata[j+2] + rightMetadata[2];
                        writeToBuffer(buffer, &newDimLength, 1);
                        rightMetadataIdx++;
                    } else {
                        writeToBuffer(buffer, &leftMetadata[j+2], 1);
                    }
                } else {
                    writeToBuffer(buffer, &leftMetadata[j], 3);
                }
            }
        } else {
            // Copy metadata entries from left and right
            // But also adjust element offset of each metadata entry from right
            uint32_t newOffset = leftMetadata[(leftMetadataLength - 1) * 3] + leftMetadata[(leftMetadataLength - 1) * 3 + 1];
            writeToBuffer<uint32_t>(buffer, &leftMetadata[0], leftMetadataLength * 3);
            for (size_t j = 0; j < rightMetadataLength * 3; j += 3) {
                writeToBuffer(buffer, &newOffset, 1);
                writeToBuffer(buffer, &rightMetadata[j+1], 2);
                newOffset += rightMetadata[j+1];
            }
            rightMetadataIdx++;
        }
        leftMetadataIdx++;
    }
    // Add elements to result
    copyElements(buffer);
    other.copyElements(buffer);

    // Add null values to result
    writeToBuffer(buffer, this->nulls, countNullBytes(this->metadata[1]));
    // Step one char back if some bits in last byte not used 
    if (getTotalNumberElements() % 8 != 0) {
        buffer -= 1;
    } 
    appendNulls(buffer, other.getNulls(), other.getTotalNumberElements(), this->metadata[1]);

    // Add string values to result
    copyStrings(buffer);
    other.copyStrings(buffer);

    return VarLen32::fromString(result);
}