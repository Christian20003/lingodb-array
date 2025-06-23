#include "../include/Array.h"

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