#include "../include/Array.h"

using lingodb::runtime::Array;

const uint8_t* Array::getElements() {
    return this->elements;
}

uint32_t Array::getNumberElements(bool withNulls) {
    if (withNulls) {
        auto size = getMetadataLength();
        return this->metadata[size*entrySize-2] + this->metadata[size*entrySize-1];
    }
    return this->numberElements;
}

uint32_t Array::getMaxDimensionSize(uint32_t dimension) {
    if (this->numberDimensions < dimension) {
        throw std::runtime_error("Requested array dimension does not exist");
    }
    auto length = this->metadataLengths[dimension-1];
    auto *entry = getFirstEntry(dimension);
    uint32_t result = 0;
    for (uint32_t i = 0; i < length * entrySize; i += entrySize) {
        if (entry[i+1] > result) result = entry[i+1];
    }
    return result;
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

int32_t Array::getHighestPosition() {
    if (this->numberElements == 0) {
        return 0;
    }
    if (type == mlir::Type::INTEGER) {
        return getMaxIndex<int32_t>();
    } else if (type == mlir::Type::BIGINTEGER) {
        return getMaxIndex<int64_t>();
    } else if (type == mlir::Type::FLOAT) {
        return getMaxIndex<float>();
    } else if (type == mlir::Type::DOUBLE) {
        return getMaxIndex<double>();
    } else if (type == mlir::Type::STRING) {
        return getMaxIndex<uint32_t>();
    } else {
        throw std::runtime_error("Array-HighestPosition: Given type is not supported in arrays");
    }
}

void Array::copyElements(char *&buffer) {
    if (type == mlir::Type::INTEGER) {
        writeToBuffer(buffer, reinterpret_cast<int32_t*>(this->elements), this->numberElements);
    } else if (type == mlir::Type::BIGINTEGER) {
        writeToBuffer(buffer, reinterpret_cast<int64_t*>(this->elements), this->numberElements);
    } else if (type == mlir::Type::FLOAT) {
        writeToBuffer(buffer, reinterpret_cast<float*>(this->elements), this->numberElements);
    } else if (type == mlir::Type::DOUBLE) {
        writeToBuffer(buffer, reinterpret_cast<double*>(this->elements), this->numberElements);
    } else if (type == mlir::Type::STRING) {
        writeToBuffer(buffer, reinterpret_cast<uint32_t*>(this->elements), this->numberElements);
    } else {
        throw std::runtime_error("Given type is not supported in arrays");
    }
}

void Array::copyElement(char *&buffer, uint32_t position) {
    if (this->numberElements <= position) {
        throw std::runtime_error("Array-Element does not exist");
    }
    if (type == mlir::Type::INTEGER) {
        int32_t *value = reinterpret_cast<int32_t*>(this->elements) + position;
        writeToBuffer(buffer, value, 1);
    } else if (type == mlir::Type::BIGINTEGER) {
        int64_t *value = reinterpret_cast<int64_t*>(this->elements) + position;
        writeToBuffer(buffer, value, 1);
    } else if (type == mlir::Type::FLOAT) {
        float *value = reinterpret_cast<float*>(this->elements) + position;
        writeToBuffer(buffer, value, 1);
    } else if (type == mlir::Type::DOUBLE) {
        double *value = reinterpret_cast<double*>(this->elements) + position;
        writeToBuffer(buffer, value, 1);
    } else if (type == mlir::Type::STRING) {
        uint32_t *value = reinterpret_cast<uint32_t*>(this->elements) + position;
        writeToBuffer(buffer, value, 1);
    } else {
        throw std::runtime_error("Given type is not supported in arrays");
    }
}

void Array::copyStrings(char *&buffer) {
    if (type != mlir::Type::STRING) {
        return;
    }
    size_t size = getStringLength();
    writeToBuffer(buffer, this->strings, size);
}

void Array::copyString(char *&buffer, uint32_t position) {
    if (this->numberElements <= position) {
        throw std::runtime_error("Array-Element does not exist");
    }
    uint32_t *sizes = reinterpret_cast<uint32_t*>(this->elements);
    uint32_t offset = 0;
    for (size_t i = 0; i < position; i++) {
        offset += sizes[i];
    }
    writeToBuffer(buffer, this->strings + offset, sizes[position]);
}

template<>
void Array::castAndCopyElement<int32_t>(char *&buffer, std::string &value) {
    try {
        int32_t castValue = std::stoi(value);
        writeToBuffer(buffer, &castValue, 1);
    } catch (std::invalid_argument exc) {
        throw std::runtime_error(value + " is not of type INTEGER");
    } catch (std::out_of_range exc) {
        throw std::runtime_error(value + " is out of range of 32-Bit INTEGER");
    }
}

template<>
void Array::castAndCopyElement<int64_t>(char *&buffer, std::string &value) {
    try {
        int64_t castValue = std::stol(value);
        writeToBuffer(buffer, &castValue, 1);
    } catch (std::invalid_argument exc) {
        throw std::runtime_error(value + " is not of type INTEGER");
    } catch (std::out_of_range exc) {
        throw std::runtime_error(value + " is out of range of 64-Bit INTEGER");
    }
}

template<>
void Array::castAndCopyElement<float>(char *&buffer, std::string &value) {
    try {
        float castValue = std::stof(value);
        writeToBuffer(buffer, &castValue, 1);
    } catch (std::invalid_argument exc) {
        throw std::runtime_error(value + " is not of type FLOAT");
    } catch (std::out_of_range exc) {
        throw std::runtime_error(value + " is out of range of FLOAT");
    }
}

template<>
void Array::castAndCopyElement<double>(char *&buffer, std::string &value) {
    try {
        double castValue = std::stod(value);
        writeToBuffer(buffer, &castValue, 1);
    } catch (std::invalid_argument exc) {
        throw std::runtime_error(value + " is not of type DOUBLE");
    } catch (std::out_of_range exc) {
        throw std::runtime_error(value + " is out of range of DOUBLE");
    }
}

template<>
void Array::castAndCopyElement<std::string>(char *&buffer, std::string &value) {
    writeToBuffer(buffer, &value, value.size());
}

uint32_t Array::getElementPosition(uint32_t position) {
    auto totalElements = getNumberElements(true);
    if (totalElements <= position) {
        throw std::runtime_error("Array-Element does not exist");
    }
    return position - countNulls(position);
}

template<>
void Array::toString<int32_t>(uint32_t position, std::string &target) {
    if (this->numberElements <= position) {
        throw std::runtime_error("Array-Element does not exist");
    }
    int32_t value = *reinterpret_cast<int32_t*>(this->elements + position * sizeof(int32_t));
    target.append(std::to_string(value));
}

template<>
void Array::toString<int64_t>(uint32_t position, std::string &target) {
    if (this->numberElements <= position) {
        throw std::runtime_error("Array-Element does not exist");
    }
    int64_t value = *reinterpret_cast<int64_t*>(this->elements + position * sizeof(int64_t));
    target.append(std::to_string(value));
}

template<>
void Array::toString<float>(uint32_t position, std::string &target) {
    if (this->numberElements <= position) {
        throw std::runtime_error("Array-Element does not exist");
    }
    float value = *reinterpret_cast<float*>(this->elements + position * sizeof(float));
    target.append(std::to_string(value));
}

template<>
void Array::toString<double>(uint32_t position, std::string &target) {
    if (this->numberElements <= position) {
        throw std::runtime_error("Array-Element does not exist");
    }
    double value = *reinterpret_cast<double*>(this->elements + position * sizeof(double));
    target.append(std::to_string(value));
}

template<>
void Array::toString<std::string>(uint32_t position, std::string &target) {
    if (this->numberElements <= position) {
        throw std::runtime_error("Array-Element does not exist");
    }
    auto *stringLengths = reinterpret_cast<uint32_t*>(this->elements);
    size_t prevLength = 0;

    for (size_t i = 0; i < (size_t) position; i++) {
        prevLength += stringLengths[i];
    }
    std::string value = std::string(this->strings + prevLength, stringLengths[position]);
    target.append("\"");
    target.append(value);
    target.append("\"");
}

bool Array::hasEmptyValue() {
    auto size = getMetadataLength();
    // Iterate over each metadata entry and check element-length and dimension-length
    for (size_t i = 0; i < size * 3; i += 3) {
        if (this->metadata[i+1] == 0 && this->metadata[i+2] == 0) {
            return true;
        }
    }
    return false;
}