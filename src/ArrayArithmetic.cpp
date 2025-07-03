#include "../include/Array.h"

using lingodb::runtime::Array;

lingodb::runtime::VarLen32 Array::operator+(Array &other) {
    if (!isNumericType()) {
        throw std::runtime_error("Array-Add: Given element type is not numeric");
    }
    if (type != other.getType()) {
        throw std::runtime_error("Array-Add: Arrays have different types");
    }
    if (hasNullValue() || other.hasNullValue()) {
        throw std::runtime_error("Array-Add: NULL values are not allowed");
    }
    if (hasEmptyValue() || other.hasEmptyValue()) {
        throw std::runtime_error("Array-Add: Empty array elements are not allowed");
    }
    if (!equalMetadata(other.getMetadata())) {
        throw std::runtime_error("Array-Add: Given arrays have different structures");
    }

    std::string result;
    auto size = getStringSize(this->numberDimensions, this->numberElements, getMetadataLength(), getNullBytes(this->numberElements), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, &this->numberDimensions, 1);
    writeToBuffer(buffer, &this->numberElements, 1);
    writeToBuffer(buffer, this->metadataLengths, this->numberDimensions + getMetadataLength() * 3);

    executeOperation<ArrayAddOperator>(this->elements, other.getElements(), this->numberElements, buffer, false, false, type);

    copyNulls(buffer, this->nulls, this->numberElements, 0);

    return VarLen32::fromString(result);
}

lingodb::runtime::VarLen32 Array::operator-(Array &other) {
    if (!isNumericType()) {
        throw std::runtime_error("Array-Sub: Given element type is not numeric");
    }
    if (type != other.getType()) {
        throw std::runtime_error("Array-Sub: Arrays have different types");
    }
    if (hasNullValue() || other.hasNullValue()) {
        throw std::runtime_error("Array-Sub: NULL values are not allowed");
    }
    if (hasEmptyValue() || other.hasEmptyValue()) {
        throw std::runtime_error("Array-Sub: Empty array elements are not allowed");
    }
    if (!equalMetadata(other.getMetadata())) {
        throw std::runtime_error("Array-Sub: Given arrays have different structures");
    }

    std::string result;
    auto size = getStringSize(this->numberDimensions, this->numberElements, getMetadataLength(), getNullBytes(this->numberElements), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, &this->numberDimensions, 1);
    writeToBuffer(buffer, &this->numberElements, 1);
    writeToBuffer(buffer, this->metadataLengths, this->numberDimensions + getMetadataLength() * 3);

    executeOperation<ArraySubOperator>(this->elements, other.getElements(), this->numberElements, buffer, false, false, type);

    copyNulls(buffer, this->nulls, this->numberElements, 0);

    return VarLen32::fromString(result);
}

lingodb::runtime::VarLen32 Array::operator*(Array &other) {
    if (!isNumericType()) {
        throw std::runtime_error("Array-Mul: Given element type is not numeric");
    }
    if (type != other.getType()) {
        throw std::runtime_error("Array-Mul: Arrays have different types");
    }
    if (hasNullValue() || other.hasNullValue()) {
        throw std::runtime_error("Array-Mul: NULL values are not allowed");
    }
    if (hasEmptyValue() || other.hasEmptyValue()) {
        throw std::runtime_error("Array-Mul: Empty array elements are not allowed");
    }
    if (!equalMetadata(other.getMetadata())) {
        throw std::runtime_error("Array-Mul: Given arrays have different structures");
    }

    std::string result;
    auto size = getStringSize(this->numberDimensions, this->numberElements, getMetadataLength(), getNullBytes(this->numberElements), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, &this->numberDimensions, 1);
    writeToBuffer(buffer, &this->numberElements, 1);
    writeToBuffer(buffer, this->metadataLengths, this->numberDimensions + getMetadataLength() * 3);

    executeOperation<ArrayMulOperator>(this->elements, other.getElements(), this->numberElements, buffer, false, false, type);

    copyNulls(buffer, this->nulls, this->numberElements, 0);

    return VarLen32::fromString(result);
}

lingodb::runtime::VarLen32 Array::operator/(Array &other) {
    if (!isNumericType()) {
        throw std::runtime_error("Array-Div: Given element type is not numeric");
    }
    if (type != other.getType()) {
        throw std::runtime_error("Array-Div: Arrays have different types");
    }
    if (hasNullValue() || other.hasNullValue()) {
        throw std::runtime_error("Array-Div: NULL values are not allowed");
    }
    if (hasEmptyValue() || other.hasEmptyValue()) {
        throw std::runtime_error("Array-Div: Empty array elements are not allowed");
    }
    if (!equalMetadata(other.getMetadata())) {
        throw std::runtime_error("Array-Div: Given arrays have different structures");
    }

    std::string result;
    auto size = getStringSize(this->numberDimensions, this->numberElements, getMetadataLength(), getNullBytes(this->numberElements), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, &this->numberDimensions, 1);
    writeToBuffer(buffer, &this->numberElements, 1);
    writeToBuffer(buffer, this->metadataLengths, this->numberDimensions + getMetadataLength() * 3);

    executeOperation<ArrayDivOperator>(this->elements, other.getElements(), this->numberElements, buffer, false, false, type);

    copyNulls(buffer, this->nulls, this->numberElements, 0);

    return VarLen32::fromString(result);
}