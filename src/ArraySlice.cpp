#include "../include/Array.h"

using lingodb::runtime::Array;

lingodb::runtime::VarLen32 Array::slice(uint32_t lowerBound, uint32_t upperBound, uint32_t dimension) {
    if (lowerBound > upperBound) {
        throw std::runtime_error("Array-Slice: Given lower-bound is larger than given upper-bound");
    }
    if (dimension > this->numberDimensions) {
        return createEmptyArray(type);
    }

    // Update metadata
    std::vector<uint32_t> metadata{0,0,0};
    std::vector<uint32_t> metadataLengths;
    std::vector<uint32_t> elementIdx;
    std::vector<bool> nulls;
    uint32_t numberElements = 0;
    uint32_t stringLengths = 0;

    metadataLengths.resize(this->numberDimensions, 0);
    metadataLengths[0] = 1;

    const auto *start = this->metadata;
    auto totalElements = metadataSlice(metadata, metadataLengths, elementIdx, lowerBound-1, upperBound-1, dimension, 1, 0, start);
    metadata[1] = totalElements;
    metadata[2] = metadataLengths[1];
    nulls.reserve(elementIdx.size());

    // Get string length and null values
    for (auto &entry : elementIdx) {
        bool isNull = checkNull(entry);
        nulls.push_back(isNull);
        if (type == mlir::Type::STRING && !isNull) {
            stringLengths += getStringLength(getElementPosition(entry));
        }
        if (!isNull) numberElements++;
    }

    // Define result string
    std::string result;
    size_t size = getStringSize(
        this->numberDimensions,
        numberElements,
        metadata.size() / 3,
        countNullBytes(totalElements),
        stringLengths,
        type
    );
    result.resize(size);
    char *buffer = result.data();

    // Write array data into string
    writeToBuffer(buffer, &this->numberDimensions, 1);
    writeToBuffer(buffer, &numberElements, 1);
    writeToBuffer(buffer, metadataLengths.data(), this->numberDimensions);
    writeToBuffer(buffer, metadata.data(), metadata.size());

    for (auto &entry : elementIdx) {
        if (!checkNull(entry)) {
            copyElement(buffer, getElementPosition(entry));
        }
    }
    castNulls(nulls, buffer);

    if (type == mlir::Type::STRING) {
        for (size_t i = 0; i < elementIdx.size(); i++) {
            auto j = elementIdx[i];
            if (!checkNull(j)) {
                copyString(buffer, getElementPosition(j));
            }
        }
    }
    return VarLen32::fromString(result);
}

uint32_t Array::metadataSlice(
    std::vector<uint32_t> &metadata, 
    std::vector<uint32_t> &lengths, 
    std::vector<uint32_t> &elements, 
    uint32_t lowerBound,
    uint32_t upperBound,
    uint32_t sliceDimension, 
    uint32_t dimension,
    uint32_t offset, 
    const uint32_t *&entry) {

    uint32_t result = 0;
    bool isSlice = dimension == sliceDimension;
    // Case for last dimension
    if (dimension == this->numberDimensions) {
        for (uint32_t i = 0; i < entry[1]; i++) {
            // If slice then proof if element is outside the interval
            if (isSlice && (i < lowerBound || i > upperBound)) {
                continue;
            }
            result++;
            elements.push_back(i + entry[0]);
        }
        return result;
    }

    // Case for other dimensions
    auto *subEntry = getChildEntry(entry, dimension);
    // Identify insert position for the metadata vector
    auto insertPos = 0;
    for (size_t i = 0; i < dimension + 1; i++) {
        if (lengths.size() > i) {
            insertPos += lengths[i] * 3;
        }
    }
    // Identify insert position for the lengths vector
    auto lengthPos = lengths.size() < dimension + 1 ? lengths.size() : dimension;
    uint32_t newOffset = offset;
    // Iterate over each child metadata entry
    for (size_t i = 0; i < entry[2] * 3; i+=3) {
        // If slice then proof if child is outside the interval
        if (isSlice && (i/3 < lowerBound || i/3 > upperBound)) {
            continue;
        }
        // Adjust metadata length of particular dimension
        *(lengths.begin() + lengthPos) += 1;
        // Add new metadata entry to corresponding vector for this child (triple)
        metadata.insert(metadata.begin() + insertPos, newOffset);
        metadata.insert(metadata.begin() + insertPos+1, 0);
        metadata.insert(metadata.begin() + insertPos+2, 0);

        auto dimChange = lengths.size() < dimension + 1 ? 0 : lengths[dimension+1];
        auto *child = subEntry + i;
        // Check child entry recursively
        auto elemLength = metadataSlice(metadata, lengths, elements, lowerBound, upperBound, sliceDimension, dimension + 1, newOffset, child);
        // Proof if null value must be adopted (not deleted from slice)
        if (elemLength == 0 && subEntry[i+1] == 1 && dimension >= sliceDimension) {
            elements.push_back(subEntry[i]);
            elemLength++;
        }
        // Adjust elem-Length and dim-Length of new metadata entry
        metadata[insertPos + 1] = elemLength;
        metadata[insertPos + 2] = lengths[dimension+1] - dimChange;
        insertPos += 3;
        newOffset += elemLength;
        result += elemLength;
    }
    return result;
}