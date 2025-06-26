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