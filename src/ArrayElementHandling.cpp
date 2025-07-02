#include "../include/Array.h"

using lingodb::runtime::Array;

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
    uint32_t *sizes = reinterpret_cast<uint32_t*>(this->elements);
    uint32_t offset = 0;
    for (size_t i = 0; i < position; i++) {
        offset += sizes[i];
    }
    writeToBuffer(buffer, this->strings + offset, sizes[position]);
}

template<>
void Array::castElement<int32_t>(std::string &value, char *&writer) {
    try {
        int32_t castValue = std::stoi(value);
        memcpy(writer, &castValue, sizeof(int32_t));
        writer += sizeof(int32_t);
    } catch (std::invalid_argument exc) {
        throw std::runtime_error(value + "is not of type INTEGER");
    } catch (std::out_of_range exc) {
        throw std::runtime_error(value + " is out of range of 32-Bit INTEGER");
    }
}

template<>
void Array::castElement<int64_t>(std::string &value, char *&writer) {
    try {
        int64_t castValue = std::stol(value);
        memcpy(writer, &castValue, sizeof(int64_t));
        writer += sizeof(int64_t);
    } catch (std::invalid_argument exc) {
        throw std::runtime_error(value + "is not of type INTEGER");
    } catch (std::out_of_range exc) {
        throw std::runtime_error(value + " is out of range of 64-Bit INTEGER");
    }
}

template<>
void Array::castElement<float>(std::string &value, char *&writer) {
    try {
        float castValue = std::stof(value);
        memcpy(writer, &castValue, sizeof(float));
        writer += sizeof(float);
    } catch (std::invalid_argument exc) {
        throw std::runtime_error(value + "is not of type FLOAT");
    } catch (std::out_of_range exc) {
        throw std::runtime_error(value + " is out of range of FLOAT");
    }
}

template<>
void Array::castElement<double>(std::string &value, char *&writer) {
    try {
        double castValue = std::stod(value);
        memcpy(writer, &castValue, sizeof(double));
        writer += sizeof(double);
    } catch (std::invalid_argument exc) {
        throw std::runtime_error(value + "is not of type DOUBLE");
    } catch (std::out_of_range exc) {
        throw std::runtime_error(value + " is out of range of DOUBLE");
    }
}

template<>
void Array::castElement<std::string>(std::string &value, char *&writer) {
    memcpy(writer, &value, value.size());
    writer += value.size();
}

void Array::appendStringValue(std::string &target, uint32_t position) {
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

uint32_t Array::getElementPosition(uint32_t position) {
    return position - countNulls(position);
}

template<>
std::string Array::toString<int32_t>(uint32_t position) {
    int32_t value = *reinterpret_cast<int32_t*>(this->elements + position * sizeof(int32_t));
    return std::to_string(value);
}

template<>
std::string Array::toString<int64_t>(uint32_t position) {
    int64_t value = *reinterpret_cast<int64_t*>(this->elements + position * sizeof(int64_t));
    return std::to_string(value);
}

template<>
std::string Array::toString<float>(uint32_t position) {
    float value = *reinterpret_cast<float*>(this->elements + position * sizeof(float));
    return std::to_string(value);
}

template<>
std::string Array::toString<double>(uint32_t position) {
    double value = *reinterpret_cast<double*>(this->elements + position * sizeof(double));
    return std::to_string(value);
}