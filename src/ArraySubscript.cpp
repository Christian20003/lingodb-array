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
        if (checkNull(elementIdx[0])) {
            return VarLen32::fromString(result);
        }
        auto position = getElementPosition(elementIdx[0]);
        if (type == mlir::Type::INTEGER) {
            result.append(toString<int32_t>(position));
        } else if (type == mlir::Type::BIGINTEGER) {
            result.append(toString<int64_t>(position));
        } else if (type == mlir::Type::FLOAT) {
            result.append(toString<float>(position));
        } else if (type == mlir::Type::DOUBLE) {
            result.append(toString<double>(position));
        } else {
            appendStringValue(result, position);
        }
        return VarLen32::fromString(result);
    }

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
    size_t size = getStringSize(
        dimension,
        numberElements,
        metadata.size() / 3,
        countNullBytes(totalElements),
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