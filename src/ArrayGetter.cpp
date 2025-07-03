#include "../include/Array.h"

using lingodb::runtime::Array;
using lingodb::runtime::VarLen32;

size_t Array::getStringSize(uint32_t dimensions, uint32_t numberElements, uint32_t metadataSize, uint32_t nullSize, uint32_t stringSize, mlir::Type type) {
    size_t size = sizeof(uint32_t) * 2 + dimensions * sizeof(uint32_t) + metadataSize * 3 * sizeof(uint32_t);
    if (type == mlir::Type::INTEGER) {
        size += numberElements * sizeof(int32_t);
    } else if (type == mlir::Type::BIGINTEGER || type == mlir::Type::STRING) {
        size += numberElements * sizeof(int64_t);
    } else if (type == mlir::Type::FLOAT) {
        size += numberElements * sizeof(float);
    } else if (type == mlir::Type::DOUBLE) {
        size += numberElements * sizeof(double);
    } else {
        throw std::runtime_error("Given type is not supported in arrays");
    }
    size += nullSize;
    size += stringSize;
    return size;
}

size_t Array::getTypeSize(mlir::Type type) {
    if (type == mlir::Type::INTEGER) {
        return sizeof(int32_t);
    } else if (type == mlir::Type::BIGINTEGER) {
        return sizeof(int64_t);
    } else if (type == mlir::Type::FLOAT) {
        return sizeof(float);
    } else if (type == mlir::Type::DOUBLE) {
        return sizeof(double);
    } else if (type == mlir::Type::STRING) {
        return sizeof(uint32_t);
    } else {
        throw std::runtime_error("Given type is not supported in arrays");
    }
}

mlir::Type Array::getType() {
    return this->type;
}

uint32_t Array::getDimension() {
    return this->numberDimensions;
}

uint32_t Array::getNumberElements(bool withNulls) {
    if (withNulls) {
        return this->metadata[1];
    }
    return this->numberElements;
}

uint32_t Array::getMetadataLength() {
    uint32_t result = 0;
    for (size_t i = 0; i < this->numberDimensions; i++) {
        result += this->metadataLengths[i];
    }
    return result;
}

uint32_t Array::getMetadataLength(uint32_t dimension) {
    if (dimension > this->numberDimensions || dimension == 0) {
        throw std::runtime_error("Array-Dimension does not exist");
    }
    return this->metadataLengths[dimension-1];
}

const uint8_t* Array::getElements() {
    return this->elements;
}

uint32_t Array::getStringLength() {
    if (type != mlir::Type::STRING) {
        return 0;
    }
    uint32_t result = 0;
    uint32_t *stringLengths = reinterpret_cast<uint32_t*>(this->elements);
    for (size_t i = 0; i < this->numberElements; i++) {
        result += stringLengths[i];
    }
    return result;
}

uint32_t Array::getStringLength(uint32_t position) {
    if (this->numberElements <= position) {
        throw std::runtime_error("Array-Element position does not exist");
    }
    if (type != mlir::Type::STRING) {
        return 0;
    }
    uint32_t *stringLengths = reinterpret_cast<uint32_t*>(this->elements);
    return stringLengths[position];
}

const uint32_t* Array::getMetadata() {
    return this->metadata;
}

const uint8_t* Array::getNulls() {
    return this->nulls;
}