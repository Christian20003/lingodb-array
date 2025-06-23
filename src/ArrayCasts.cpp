#include "../include/Array.h"

template<>
void Array::castElement<int32_t>(std::string &value, char *writer) {
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
void Array::castElement<int64_t>(std::string &value, char *writer) {
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
void Array::castElement<float>(std::string &value, char *writer) {
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
void Array::castElement<double>(std::string &value, char *writer) {
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

void Array::castNulls(std::vector<bool> &nulls, char *writer) {
    for (size_t i = 0; i < nulls.size(); i++) {
        if (i != 0 && i % 8 == 0) {
            writer += 1;
        }
        if (nulls[i]) {
            uint8_t index = i % 8;
            uint8_t shift = 8 - index - 1;
            *writer |= (1 << shift);
        }
    }
    writer += 1;
}

template<>
void Array::castElement<std::string>(std::string &value, char *writer) {
    memcpy(writer, &value, value.size());
    writer += value.size();
}

template<>
std::string Array::toString<int32_t>(int32_t position) {
    return std::to_string(*(this->elements + position * sizeof(int32_t)));
}

template<>
std::string Array::toString<int64_t>(int32_t position) {
    return std::to_string(*(this->elements + position * sizeof(int64_t)));
}

template<>
std::string Array::toString<float>(int32_t position) {
    return std::to_string(*(this->elements + position * sizeof(float)));
}

template<>
std::string Array::toString<double>(int32_t position) {
    return std::to_string(*(this->elements + position * sizeof(double)));
}