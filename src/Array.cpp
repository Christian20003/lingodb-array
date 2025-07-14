#include "../include/Array.h"
#include <iostream>

using lingodb::runtime::Array;

Array::Array(std::string &array, mlir::Type type) {
    // Check if string is not empty
    if (array.size() == 0) {
        throw std::runtime_error("Array is empty");
    }
    // Assign each pointer and value its position / value
    char *data = array.data();
    this->numberDimensions = *reinterpret_cast<uint32_t*>(data);
    data += sizeof(uint32_t);
    this->numberElements = *reinterpret_cast<uint32_t*>(data);
    data += sizeof(uint32_t);
    this->indices = reinterpret_cast<int32_t*>(data);
    data += sizeof(int32_t) * this->numberDimensions;
    this->metadataLengths = reinterpret_cast<uint32_t*>(data);
    data += this->numberDimensions * sizeof(uint32_t);
    this->metadata = reinterpret_cast<uint32_t*>(data);
    for (size_t i = 0; i < this->numberDimensions; i++) {
        data += this->metadataLengths[i] * sizeof(uint32_t);
    }
    this->elements = reinterpret_cast<uint8_t*>(data);
    data += this->numberElements * getTypeSize(type);
    this->nulls = reinterpret_cast<uint8_t*>(data);
    data += getNullBytes(getNumberElements(true));
    this->strings = data;
    this->type = type;
    if (type == mlir::Type::INTEGER) {
        printData();
    }
}

lingodb::runtime::VarLen32 Array::createEmptyArray(mlir::Type type) {
    std::string result = "";
    uint32_t dimension = 1;
    uint32_t elements = 0;
    size_t size = getStringSize(1, 0, 1, 0, 0, type);
    result.resize(size);
    char *buffer = &result[0];

    writeToBuffer(buffer, &dimension, 1);
    writeToBuffer(buffer, &elements, 1);
    writeToBuffer(buffer, &dimension, 1);
    writeToBuffer(buffer, &elements, 1);
    writeToBuffer(buffer, &elements, 1);
    writeToBuffer(buffer, &elements, 1);

    return VarLen32::fromString(result);
}

bool Array::isNumericType() {
    if (type == mlir::Type::INTEGER ||
        type == mlir::Type::BIGINTEGER ||
        type == mlir::Type::FLOAT ||
        type == mlir::Type::DOUBLE) {
            return true;
        }
    return false;
}

bool Array::isFloatingPointType() {
    if (type == mlir::Type::FLOAT || type == mlir::Type::DOUBLE) {
        return true;
    }
    return false;
}

size_t Array::getStringSize(uint32_t dimensions, uint32_t numberElements, uint32_t metadataSize, uint32_t nullSize, uint32_t stringSize, mlir::Type type) {
    size_t size = sizeof(uint32_t) * 2 + dimensions * sizeof(int32_t) + dimensions * sizeof(uint32_t) + metadataSize * sizeof(uint32_t);
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

void Array::printData() {
    size_t metadataLen = 0;
    std::cout << "INDICES:" << std::endl;
    for (size_t i = 0; i < this->numberDimensions; i++) {
        std::cout << this->indices[i] << ",";
    }
    std::cout << std::endl;
    std::cout << "METADATA-LENGTHS:" << std::endl;
    for (size_t i = 0; i < this->numberDimensions; i++) {
        std::cout << this->metadataLengths[i] << ",";
        metadataLen += this->metadataLengths[i];
    }
    std::cout << std::endl;
    std::cout << "METADATA-ENTRIES:" << std::endl;
    for (size_t i = 0; i < metadataLen; i++) {
        std::cout << this->metadata[i] << ","; 
    }
    std::cout << std::endl;
    printNulls();
}

void Array::printNulls() {
    std::cout << "NULLS:" << std::endl;
    for (size_t i = 0; i < getNumberElements(true); i++) {
        std::cout << isNull(i) << ",";
    }
    std::cout << std::endl;
}