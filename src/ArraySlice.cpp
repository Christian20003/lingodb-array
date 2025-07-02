#include "../include/Array.h"

using lingodb::runtime::Array;

lingodb::runtime::VarLen32 Array::slice(uint32_t lowerBound, uint32_t upperBound, uint32_t dimension) {
    if (lowerBound > upperBound) {
        throw std::runtime_error("Array-Slice: Given lower-bound is larger than given upper-bound");
    }
    if (dimension > this->numberDimensions) {
        return createEmptyArray(type);
    }

    std::vector<uint32_t> metadata{0,0,0};
    std::vector<uint32_t> metadataLengths{1};
    std::vector<uint32_t> elementIdx;
    std::vector<bool> nulls;
    uint32_t stringLengths = 0;

    const auto *start = this->metadata;
    auto totalElements = metadataSlice(metadata, metadataLengths, elementIdx, lowerBound-1, upperBound-1, dimension, 1, 0, start);
    metadata[1] = totalElements;
    metadata[2] = metadataLengths[1];

    for (auto &entry : elementIdx) {
        if (checkNull(entry)) {
            nulls.push_back(true);
        } else {
            if (type == mlir::Type::STRING) {
                stringLengths += entry;
            }
            nulls.push_back(false);
        }
    }

    std::string result;
    size_t size = getStringSize(
        this->numberDimensions,
        totalElements - std::count(nulls.begin(), nulls.end(), true),
        metadata.size() / 3,
        countNullBytes(totalElements),
        stringLengths,
        type
    );
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, &this->numberDimensions, 1);
    writeToBuffer(buffer, &totalElements, 1);
    writeToBuffer(buffer, metadataLengths.data(), this->numberDimensions);
    writeToBuffer(buffer, metadata.data(), metadata.size());

    for (auto &entry : elementIdx) {
        copyElement(buffer, entry);
    }
    castNulls(nulls, buffer);

    if (type == mlir::Type::STRING) {
        char *values = this->strings;
        uint32_t *sizes = reinterpret_cast<uint32_t*>(this->elements);
        size_t idx = 0;
        for (size_t i = 0; i < getTotalNumberElements(); i++) {
            if (i == elementIdx[idx] && !checkNull(i)) {
                writeToBuffer(buffer, values, sizes[getElementPosition(i)]);
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
    if (dimension == this->numberDimensions) {
        for (uint32_t i = entry[0]; i < entry[0] + entry[1]; i++) {
            if (dimension == sliceDimension && (i < lowerBound || i > upperBound)) {
                continue;
            }
            result++;
            elements.push_back(i);
        }
        return result;
    }

    auto *subEntry = getChildEntry(entry, dimension);
    auto insertPos = 0;
    for (size_t i = 0; i < dimension + 1; i++) {
        if (lengths.size() > i) {
            insertPos += lengths[i] * 3;
        }
    }
    auto lengthPos = lengths.size() < dimension + 1 ? lengths.size() : dimension;
    uint32_t newOffset = offset;
    for (size_t i = 0; i < entry[2] * 3; i+=3) {
        if (dimension == sliceDimension && (i/3 < lowerBound || i/3 > upperBound)) {
            continue;
        }
        if (lengths.size() < dimension + 1) {
            lengths.insert(lengths.begin() + lengthPos, 1);
        } else {
            *(lengths.begin() + lengthPos) += 1;
        }
        metadata.insert(metadata.begin() + insertPos, newOffset);
        metadata.insert(metadata.begin() + insertPos+1, 0);
        metadata.insert(metadata.begin() + insertPos+2, 0);
        if (dimension == sliceDimension && (subEntry[i+2] < lowerBound || subEntry[i+2] < upperBound)) {
            insertPos += 3;
            continue;
        }
        auto dimChange = lengths.size() < dimension + 1 ? 0 : lengths[dimension+1];
        auto *child = subEntry + i;
        auto elemLength = metadataSlice(metadata, lengths, elements, lowerBound, upperBound, sliceDimension, dimension + 1, newOffset, child);
        if (elemLength == 0 && subEntry[i+1] == 1) {
            elemLength++;
        }
        metadata[insertPos + 1] = elemLength;
        metadata[insertPos + 2] = lengths[dimension+1] - dimChange;
        insertPos += 3;
        newOffset += elemLength;
        result += elemLength;
    }
    return result;
}