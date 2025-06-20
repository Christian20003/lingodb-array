#include <cstring>
#include "../include/Array.h"

template<>
void Array::castElements<int32_t>(std::vector<std::string> &original) {
    this->elements = new uint8_t[original.size() * sizeof(int32_t)];
    for (size_t i = 0; i < original.size(); i++) {
        try {
            int32_t value = std::stoi(original[i]);
            std::memcpy(this->elements + i * sizeof(int32_t), &value, sizeof(int32_t));
        } catch (std::invalid_argument exc) {
            throw std::runtime_error(original[i] + "is not of type INTEGER");
        } catch (std::out_of_range exc) {
            throw std::runtime_error(original[i] + " is out of range of 32-Bit INTEGER");
        }
    }
}

template<>
void Array::castElements<int64_t>(std::vector<std::string> &original) {
    this->elements = new uint8_t[original.size() * sizeof(int64_t)];
    for (size_t i = 0; i < original.size(); i++) {
        try {
            int64_t value = std::stoi(original[i]);
            std::memcpy(this->elements + i * sizeof(int64_t), &value, sizeof(int64_t));
        } catch (std::invalid_argument exc) {
            throw std::runtime_error(original[i] + "is not of type INTEGER");
        } catch (std::out_of_range exc) {
            throw std::runtime_error(original[i] + " is out of range of 64-Bit INTEGER");
        }
    }
}

template<>
void Array::castElements<float>(std::vector<std::string> &original) {
    this->elements = new uint8_t[original.size() * sizeof(float)];
    for (size_t i = 0; i < original.size(); i++) {
        try {
            float value = std::stoi(original[i]);
            std::memcpy(this->elements + i * sizeof(float), &value, sizeof(float));
        } catch (std::invalid_argument exc) {
            throw std::runtime_error(original[i] + "is not of type FLOAT");
        } catch (std::out_of_range exc) {
            throw std::runtime_error(original[i] + " is out of range of FLOAT");
        }
    }
}

template<>
void Array::castElements<double>(std::vector<std::string> &original) {
    this->elements = new uint8_t[original.size() * sizeof(double)];
    for (size_t i = 0; i < original.size(); i++) {
        try {
            double value = std::stoi(original[i]);
            std::memcpy(this->elements + i * sizeof(double), &value, sizeof(double));
        } catch (std::invalid_argument exc) {
            throw std::runtime_error(original[i] + "is not of type DOUBLE");
        } catch (std::out_of_range exc) {
            throw std::runtime_error(original[i] + " is out of range of DOUBLE");
        }
    }
}

template<>
void Array::castElements<std::string>(std::vector<std::string> &original) {
    size_t size = 0;
    for (auto &element : original) {
        size += element.size();
    }
    this->strings = new char[size];
    memcpy(this->strings, &original, original.size());
}