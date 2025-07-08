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

    executeBinaryOperation<ArrayAddOperator>(this->elements, other.getElements(), this->numberElements, buffer, false, false, type);

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

    executeBinaryOperation<ArraySubOperator>(this->elements, other.getElements(), this->numberElements, buffer, false, false, type);

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

    executeBinaryOperation<ArrayMulOperator>(this->elements, other.getElements(), this->numberElements, buffer, false, false, type);

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

    executeBinaryOperation<ArrayDivOperator>(this->elements, other.getElements(), this->numberElements, buffer, false, false, type);

    copyNulls(buffer, this->nulls, this->numberElements, 0);

    return VarLen32::fromString(result);
}

template<>
lingodb::runtime::VarLen32 Array::scalarAdd(int32_t value) {
    if (type != mlir::Type::INTEGER) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (32-bit)");
    }
    return executeScalarOperation<int32_t, ArrayAddOperator>(value, true);
}

template<>
lingodb::runtime::VarLen32 Array::scalarAdd(int64_t value) {
    if (type != mlir::Type::BIGINTEGER) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (64-bit)");
    }
    return executeScalarOperation<int64_t, ArrayAddOperator>(value, true);
}

template<>
lingodb::runtime::VarLen32 Array::scalarAdd(float value) {
    if (type != mlir::Type::FLOAT) {
        throw std::runtime_error("Array-Add: Array elements are not of type float");
    }
    return executeScalarOperation<float, ArrayAddOperator>(value, true);
}

template<>
lingodb::runtime::VarLen32 Array::scalarAdd(double value) {
    if (type != mlir::Type::DOUBLE) {
        throw std::runtime_error("Array-Add: Array elements are not of type double");
    }
    return executeScalarOperation<double, ArrayAddOperator>(value, true);
}

template<>
lingodb::runtime::VarLen32 Array::scalarSub(int32_t value, bool isLeft) {
    if (type != mlir::Type::INTEGER) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (32-bit)");
    }
    return executeScalarOperation<int32_t, ArraySubOperator>(value, isLeft);
}

template<>
lingodb::runtime::VarLen32 Array::scalarSub(int64_t value, bool isLeft) {
    if (type != mlir::Type::BIGINTEGER) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (64-bit)");
    }
    return executeScalarOperation<int64_t, ArraySubOperator>(value, isLeft);
}

template<>
lingodb::runtime::VarLen32 Array::scalarSub(float value, bool isLeft) {
    if (type != mlir::Type::FLOAT) {
        throw std::runtime_error("Array-Add: Array elements are not of type float");
    }
    return executeScalarOperation<float, ArraySubOperator>(value, isLeft);
}

template<>
lingodb::runtime::VarLen32 Array::scalarSub(double value, bool isLeft) {
    if (type != mlir::Type::DOUBLE) {
        throw std::runtime_error("Array-Add: Array elements are not of type double");
    }
    return executeScalarOperation<double, ArraySubOperator>(value, isLeft);
}

template<>
lingodb::runtime::VarLen32 Array::scalarMul(int32_t value) {
    if (type != mlir::Type::INTEGER) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (32-bit)");
    }
    return executeScalarOperation<int32_t, ArrayMulOperator>(value, true);
}

template<>
lingodb::runtime::VarLen32 Array::scalarMul(int64_t value) {
    if (type != mlir::Type::BIGINTEGER) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (64-bit)");
    }
    return executeScalarOperation<int64_t, ArrayMulOperator>(value, true);
}

template<>
lingodb::runtime::VarLen32 Array::scalarMul(float value) {
    if (type != mlir::Type::FLOAT) {
        throw std::runtime_error("Array-Add: Array elements are not of type float");
    }
    return executeScalarOperation<float, ArrayMulOperator>(value, true);
}

template<>
lingodb::runtime::VarLen32 Array::scalarMul(double value) {
    if (type != mlir::Type::DOUBLE) {
        throw std::runtime_error("Array-Add: Array elements are not of type double");
    }
    return executeScalarOperation<double, ArrayMulOperator>(value, true);
}

template<>
lingodb::runtime::VarLen32 Array::scalarDiv(int32_t value, bool isLeft) {
    if (type != mlir::Type::INTEGER) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (32-bit)");
    }
    return executeScalarOperation<int32_t, ArrayDivOperator>(value, isLeft);
}

template<>
lingodb::runtime::VarLen32 Array::scalarDiv(int64_t value, bool isLeft) {
    if (type != mlir::Type::BIGINTEGER) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (64-bit)");
    }
    return executeScalarOperation<int64_t, ArrayDivOperator>(value, isLeft);
}

template<>
lingodb::runtime::VarLen32 Array::scalarDiv(float value, bool isLeft) {
    if (type != mlir::Type::FLOAT) {
        throw std::runtime_error("Array-Add: Array elements are not of type float");
    }
    return executeScalarOperation<float, ArrayDivOperator>(value, isLeft);
}

template<>
lingodb::runtime::VarLen32 Array::scalarDiv(double value, bool isLeft) {
    if (type != mlir::Type::DOUBLE) {
        throw std::runtime_error("Array-Add: Array elements are not of type double");
    }
    return executeScalarOperation<double, ArrayDivOperator>(value, isLeft);
}