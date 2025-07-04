// This file contains implementations of template functions that have not been specialised
#include <type_traits>
#include "Array.h"

using lingodb::runtime::Array;

template<class TYPE>
void Array::writeToBuffer(char *&buffer, const TYPE *data, uint32_t size) {
    memcpy(buffer, data, sizeof(TYPE) * size);
    buffer += sizeof(TYPE) * size;
}

template<class TYPE, class OP>
lingodb::runtime::VarLen32 Array::executeScalarOperation(TYPE value, bool isLeft) {
    // Define result string size (does not change)
    std::string result;
    auto size = getStringSize(this->numberDimensions, this->numberElements, getMetadataLength(), getNullBytes(this->metadata[1]), 0, type);
    result.resize(size);
    char *buffer = result.data();
    // Write every content to the result (does not change except elements)
    writeToBuffer(buffer, &this->numberDimensions, 1);
    writeToBuffer(buffer, &this->numberElements, 1);
    writeToBuffer(buffer, this->metadataLengths, this->numberDimensions + getMetadataLength() * 3);
    const uint8_t *left = isLeft ? reinterpret_cast<uint8_t*>(&value) : this->elements;
    const uint8_t *right = isLeft ? this->elements : reinterpret_cast<uint8_t*>(&value);
    executeOperation<OP>(left, right, this->numberElements, buffer, isLeft, !isLeft, type);
    copyNulls(buffer, this->nulls, this->metadata[1], 0);
    return VarLen32::fromString(result);
}

template<class OP>
void Array::executeOperation(const uint8_t *left, const uint8_t *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight, mlir::Type type) {
    if (type == mlir::Type::INTEGER) {
        auto *leftVal = reinterpret_cast<const int32_t*>(left);
        auto *rightVal = reinterpret_cast<const int32_t*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else if (type == mlir::Type::BIGINTEGER) {
        auto *leftVal = reinterpret_cast<const int64_t*>(left);
        auto *rightVal = reinterpret_cast<const int64_t*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else if (type == mlir::Type::FLOAT) {
        auto *leftVal = reinterpret_cast<const float*>(left);
        auto *rightVal = reinterpret_cast<const float*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else if (type == mlir::Type::DOUBLE) {
        auto *leftVal = reinterpret_cast<const double*>(left);
        auto *rightVal = reinterpret_cast<const double*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else {
        throw std::runtime_error("Array-Type is not supported");
    }
}

template<class TYPE>
lingodb::runtime::VarLen32 Array::appendElement(TYPE value) {
    std::string result = "";
    size_t size = getStringSize(
        this->numberDimensions, 
        this->numberElements + 1,
        getMetadataLength(),
        getNullBytes(getNumberElements(true) + 1),
        0,
        type
    );
    result.resize(size);
    char *buffer = result.data();

    uint32_t numberElements = this->numberElements + 1;
    writeToBuffer(buffer, &this->numberDimensions, 1);
    writeToBuffer(buffer, &numberElements, 1);
    writeToBuffer(buffer, this->metadataLengths, this->numberDimensions);
    auto *test = reinterpret_cast<uint32_t *>(buffer);
    for (size_t i = 1; i <= this->numberDimensions; i++) {
        const uint32_t *metadata = getFirstEntry(i);
        uint32_t metadataLength = getMetadataLength(i);
        uint32_t newLength = metadata[metadataLength * 3 - 2] + 1;
        writeToBuffer(buffer, metadata, (metadataLength - 1) * 3);
        writeToBuffer(buffer, &metadata[metadataLength * 3 - 3], 1);
        writeToBuffer(buffer, &newLength, 1);
        writeToBuffer(buffer, &metadata[metadataLength * 3 - 1], 1);
    }
    copyElements(buffer);
    writeToBuffer(buffer, &value, 1);
    writeToBuffer(buffer, this->nulls, getNullBytes(this->metadata[1]));
    return VarLen32::fromString(result);
}

template<class TYPE, class ARRAYTYPE>
lingodb::runtime::VarLen32 Array::generate(TYPE *value, Array &structure, mlir::Type type, uint32_t stringSize) {
    const ARRAYTYPE *elements = reinterpret_cast<const ARRAYTYPE*>(structure.getElements());
    auto size = structure.getNumberElements();

    // Get information about how many time the values must copied and
    // How many metadata entries are required in total and in each dimension 
    uint32_t elementCopies = 1;
    uint32_t metadataSize = 1;
    std::vector<uint32_t> metadataLengths{1};
    for (size_t i = 0; i < size; i++) {
        elementCopies *= static_cast<uint32_t>(elements[i]);
        if (i+1 < size) {
            metadataLengths.push_back(elementCopies);
            metadataSize += elementCopies; 
        }
    }

    // Prepare result string
    std::string result;
    auto resultSize = getStringSize(size, elementCopies, metadataSize, getNullBytes(elementCopies), stringSize*elementCopies, type);
    result.resize(resultSize);
    char *buffer = result.data();

    // Write content to result
    writeToBuffer(buffer, &size, 1);
    writeToBuffer(buffer, &elementCopies, 1);
    writeToBuffer(buffer, metadataLengths.data(), metadataLengths.size());
    uint32_t elementLength = elementCopies;
    // Iterate over each element of the structure array
    for (size_t i = 0; i < size; i++) {
        uint32_t elementOffset = 0;
        uint32_t dimensionLength = static_cast<uint32_t>(elements[i]);
        uint32_t length = metadataLengths[i];
        // Iterate over each metadata entry that is required for the result array
        for (size_t j = 0; j < length; j++) {
            writeToBuffer(buffer, &elementOffset, 1);
            if (i+1 < size) {
                writeToBuffer(buffer, &elementLength, 1);
                writeToBuffer(buffer, &dimensionLength, 1);
            } else {
                writeToBuffer(buffer, &dimensionLength, 1);
                buffer += sizeof(uint32_t);
            }
            elementOffset += elementLength;
        }
        elementLength = elementLength / dimensionLength; 
    }
    // Check if array element type is string
    if (!std::is_same<TYPE, char>::value){
        // If false, copy only n times the value
        for (size_t i = 0; i < elementCopies; i++) {
            writeToBuffer(buffer, value, 1);
        }
    } else {
        // If true, copy n times string length and n times the string itself
        for (size_t i = 0; i < elementCopies; i++) {
            writeToBuffer(buffer, &stringSize, 1);
        }
        buffer += getNullBytes(elementCopies);
        for (size_t i = 0; i < elementCopies; i++) {
            writeToBuffer(buffer, value, stringSize);
        }
    }

    return VarLen32::fromString(result);
}