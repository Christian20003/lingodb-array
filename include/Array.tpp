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
    auto totalElements = getSize(true);
    std::string result;
    auto size = getStringSize(this->dimensions, this->size, getWidthSize(), getNullBytes(totalElements), 0, type);
    result.resize(size);
    char *buffer = result.data();
    // Write every content to the result (does not change except elements)
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, getWidthSize());
    auto *left = isLeft ? reinterpret_cast<const uint8_t*>(&value) : this->elements;
    auto *right = isLeft ? this->elements : reinterpret_cast<const uint8_t*>(&value);
    executeBinaryOperation<OP>(left, right, this->size, buffer, isLeft, !isLeft, this->type);
    copyNulls(buffer, this->nulls, totalElements, 0);
    return VarLen32::fromString(result);
}

template<class OP>
lingodb::runtime::VarLen32 Array::executeActivationFunction() {
    // Define result string size (does not change)
    auto totalElements = getSize(true);
    std::string result;
    auto size = getStringSize(this->dimensions, this->size, getWidthSize(), getNullBytes(totalElements), 0, type);
    result.resize(size);
    char *buffer = result.data();
    // Write every content to the result (does not change except elements)
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, getWidthSize());
    auto *data = this->elements;
    executeUnaryOperation<OP>(data, this->size, buffer, this->type);
    copyNulls(buffer, this->nulls, totalElements, 0);
    return VarLen32::fromString(result);
}

template<class OP>
void Array::executeBinaryOperation(const uint8_t *left, const uint8_t *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight, uint8_t type) {
    if (type == ArrayType::INTEGER32) {
        auto *leftVal = reinterpret_cast<const int32_t*>(left);
        auto *rightVal = reinterpret_cast<const int32_t*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else if (type == ArrayType::INTEGER64) {
        auto *leftVal = reinterpret_cast<const int64_t*>(left);
        auto *rightVal = reinterpret_cast<const int64_t*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else if (type == ArrayType::FLOAT) {
        auto *leftVal = reinterpret_cast<const float*>(left);
        auto *rightVal = reinterpret_cast<const float*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else if (type == ArrayType::DOUBLE) {
        auto *leftVal = reinterpret_cast<const double*>(left);
        auto *rightVal = reinterpret_cast<const double*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else {
        throw std::runtime_error("Array-Type is not supported");
    }
}

template<class OP>
void Array::executeUnaryOperation(const uint8_t *data, uint32_t size, char *&buffer, uint8_t type) {
    if (type == ArrayType::INTEGER32) {
        auto *values = reinterpret_cast<const int32_t*>(data);
        OP::Operator(values, size, buffer);
    } else if (type == ArrayType::INTEGER64) {
        auto *values = reinterpret_cast<const int64_t*>(data);
        OP::Operator(values, size, buffer);
    } else if (type == ArrayType::FLOAT) {
        auto *values = reinterpret_cast<const float*>(data);
        OP::Operator(values, size, buffer);
    } else if (type == ArrayType::DOUBLE) {
        auto *values = reinterpret_cast<const double*>(data);
        OP::Operator(values, size, buffer);
    } else {
        throw std::runtime_error("Array-Type is not supported");
    }
}

template<class TYPE>
lingodb::runtime::VarLen32 Array::appendElement(TYPE value) {
    // Variables that will change in new array
    auto numberElements = this->size + 1;
    auto nullBytes = getNullBytes(getSize(true) + 1);
    auto widthSize = getWidthSize();
    auto lastWidth = this->widths[widthSize-1] + 1;

    std::string result;
    size_t size = getStringSize(this->dimensions, numberElements, widthSize, nullBytes, 0, this->type);
    result.resize(size);
    char *buffer = result.data();

    // Write complete content to result string
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &numberElements, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, widthSize-1);
    writeToBuffer(buffer, &lastWidth, 1);
    copyElements(buffer);
    writeToBuffer(buffer, &value, 1);
    writeToBuffer(buffer, this->nulls, nullBytes);
    return VarLen32::fromString(result);
}

/* template<class TYPE, class ARRAYTYPE>
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

template<class ARRAYTYPE>
lingodb::runtime::VarLen32 Array::generate(Array &structure, mlir::Type type) {
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
    auto resultSize = getStringSize(size, 0, metadataSize, getNullBytes(elementCopies), 0, type);
    result.resize(resultSize);
    char *buffer = result.data();

    uint32_t notNullValues = 0;
    // Write content to result
    writeToBuffer(buffer, &size, 1);
    writeToBuffer(buffer, &notNullValues, 1);
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
    // Write null bits
    for (size_t i = 0; i < elementCopies; i++) {
        // Check if new null byte must be selected
        if (i != 0 && i % 8 == 0) {
            buffer++;
        }
        uint8_t index = i % 8;
        uint8_t shift = 8 - index - 1;
        *buffer |= (1 << shift);
    }

    return VarLen32::fromString(result);
} */

template<class TYPE>
int32_t Array::getMaxIndex() {
    auto *elements = reinterpret_cast<const TYPE*>(this->elements);
    int32_t result = -1;
    auto size = getSize(true);

    for (uint32_t i = 0; i < size; i++) {
        if (isNull(i)) continue;
        uint32_t position = getElementPosition(i);
        if (result == -1) result = i;
        uint32_t resultPosition = getElementPosition(result);
        if (elements[position] > elements[resultPosition]) result = i;
    }

    return result;
}