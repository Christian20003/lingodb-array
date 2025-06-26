#include "../include/Array.h"
#include <iostream>

using lingodb::runtime::Array;

Array::Array(std::string &array, mlir::Type type) {
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
        printData();
    }
}

void Array::printData() {
    size_t metadataLen = 0;
    for (size_t i = 0; i < this->numberDimensions; i++) {
        std::cout << this->metadataLengths[i] << ",";
        metadataLen += this->metadataLengths[i];
    }
    std::cout << std::endl;
    for (size_t i = 0; i < metadataLen * 3; i += 3) {
        std::cout << "{" << this->metadata[i] << ":" <<this->metadata[i+1] << ":" << this->metadata[i+2] << "},"; 
    }
    std::cout << std::endl;
}