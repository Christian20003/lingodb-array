#include "../include/Array.h"

using lingodb::runtime::Array;

lingodb::runtime::VarLen32 Array::operator[](uint32_t position) {
    
    std::string result = "";
    // Update metadata
    std::vector<uint32_t> metadata{0,0,0};
    std::vector<uint32_t> metadataLengths;
    std::vector<uint32_t> elementIdx;
    std::vector<bool> nulls;
    uint32_t numberElements = 0;
    uint32_t dimension = this->numberDimensions - 1;
    uint32_t stringLengths = 0;

    metadataLengths.resize(this->numberDimensions, 0);
    metadataLengths[0] = 1;

    const auto *start = this->metadata;
    auto totalElements = metadataSlice(metadata, metadataLengths, elementIdx, position-1, position-1, 1, 1, 0, start);
    // Remove first entry of metadata because result dimension is reduced by one
    metadataLengths.erase(metadataLengths.begin());
    metadata.erase(metadata.begin());
    metadata.erase(metadata.begin());
    metadata.erase(metadata.begin());
    nulls.reserve(elementIdx.size());

    // If no element could be found, return empty string (TODO: Inside database map to NULL)
    if (metadata[1] == 0 && metadata[2] == 0 && elementIdx.size() == 0) {
        return VarLen32::fromString(result);
    }
    // If array has only a single dimension, return this element without array data.
    if (this->numberDimensions == 1) {
        if (isNull(elementIdx[0])) {
            return VarLen32::fromString(result);
        }
        auto position = getElementPosition(elementIdx[0]);
        if (type == mlir::Type::INTEGER) {
            toString<int32_t>(position, result);
        } else if (type == mlir::Type::BIGINTEGER) {
            toString<int64_t>(position, result);
        } else if (type == mlir::Type::FLOAT) {
            toString<float>(position, result);
        } else if (type == mlir::Type::DOUBLE) {
            toString<double>(position, result);
        } else {
            toString<std::string>(position, result);
        }
        return VarLen32::fromString(result);
    }

    // Get string length and null values
    for (auto &entry : elementIdx) {
        bool null = isNull(entry);
        nulls.push_back(null);
        if (type == mlir::Type::STRING && !null) {
            stringLengths += getStringLength(getElementPosition(entry));
        }
        if (!null) numberElements++;
    }

    // Define result string
    size_t size = getStringSize(
        dimension,
        numberElements,
        metadata.size() / 3,
        getNullBytes(totalElements),
        stringLengths,
        type
    );
    result.resize(size);
    char *buffer = result.data();

    // Write array data into string
    writeToBuffer(buffer, &dimension, 1);
    writeToBuffer(buffer, &numberElements, 1);
    writeToBuffer(buffer, metadataLengths.data(), dimension);
    writeToBuffer(buffer, metadata.data(), metadata.size());

    for (auto &entry : elementIdx) {
        if (!isNull(entry)) {
            copyElement(buffer, getElementPosition(entry));
        }
    }
    copyNulls(buffer, nulls);

    if (type == mlir::Type::STRING) {
        for (size_t i = 0; i < elementIdx.size(); i++) {
            auto j = elementIdx[i];
            if (!isNull(j)) {
                copyString(buffer, getElementPosition(j));
            }
        }
    }
    return VarLen32::fromString(result);
}