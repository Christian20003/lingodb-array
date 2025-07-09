#include "../include/Array.h"
#include <iostream>

using lingodb::runtime::Array;

Array::Array(std::string &array, mlir::Type type) {
    if (array.size() == 0) {
        throw std::runtime_error("Array is empty");
    }
    char *data = array.data();
    this->numberDimensions = *reinterpret_cast<uint32_t*>(data);
    data += sizeof(uint32_t);
    this->numberElements = *reinterpret_cast<uint32_t*>(data);
    data += sizeof(uint32_t);
    this->metadataLengths = reinterpret_cast<uint32_t*>(data);
    data += this->numberDimensions * sizeof(uint32_t);
    this->metadata = reinterpret_cast<uint32_t*>(data);
    for (size_t i = 0; i < this->numberDimensions; i++) {
        data += this->metadataLengths[i] * 3 * sizeof(uint32_t);
    }
    this->elements = reinterpret_cast<uint8_t*>(data);
    data += this->numberElements * getTypeSize(type);
    this->nulls = reinterpret_cast<uint8_t*>(data);
    data += (int) std::ceil((double) this->numberElements / 8);
    this->strings = data;
    this->type = type;
    if (type == mlir::Type::INTEGER) {
        auto *test = reinterpret_cast<int32_t*>(elements);
        printData();
    }
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

void Array::printData() {
    size_t metadataLen = 0;
    std::cout << "METADATA-LENGTHS:" << std::endl;
    for (size_t i = 0; i < this->numberDimensions; i++) {
        std::cout << this->metadataLengths[i] << ",";
        metadataLen += this->metadataLengths[i];
    }
    std::cout << std::endl;
    std::cout << "METADATA-ENTRIES:" << std::endl;
    for (size_t i = 0; i < metadataLen * 3; i += 3) {
        std::cout << "{" << this->metadata[i] << ":" <<this->metadata[i+1] << ":" << this->metadata[i+2] << "},"; 
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