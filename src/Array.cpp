#include "../include/Array.h"

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
        data += this->metadataLengths[i] * 2 * sizeof(uint32_t);
    }
    this->elements = reinterpret_cast<uint8_t*>(data);
    data += this->numberElements * getTypeSize(type);
    this->nulls = reinterpret_cast<uint8_t*>(data);
    data += (int) std::ceil((double) this->numberElements / 8);
    this->strings = data;
}