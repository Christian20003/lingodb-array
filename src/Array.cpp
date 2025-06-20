#include "../include/Array.h"

Array::Array(std::string array, mlir::Type type) {
    std::vector<std::string> data;
    parseString(array, data, type);
    createElements(data, type);
}

Array::~Array() {
    delete[] this->metadataLengths;
    delete[] this->metadata;
    if (this->elements != nullptr) {
        delete[] this->elements;
    }
    if (this->nulls != nullptr) {
        delete[] this->nulls;
    }
    if (this->strings != nullptr) {
        delete[] this->strings;
    }
}