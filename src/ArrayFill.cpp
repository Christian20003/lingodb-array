#include "../include/Array.h"

using lingodb::runtime::Array;

template<>
lingodb::runtime::VarLen32 Array::fill(int32_t &value, Array &structure) {
    if (structure.getNumberElements() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == mlir::Type::INTEGER) {
        return generate<int32_t, int32_t>(&value, structure, mlir::Type::INTEGER);
    } else if (structure.getType() == mlir::Type::BIGINTEGER) {
        return generate<int32_t, int64_t>(&value, structure, mlir::Type::INTEGER);
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}

template<>
lingodb::runtime::VarLen32 Array::fill(int64_t &value, Array &structure) {
    if (structure.getNumberElements() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == mlir::Type::INTEGER) {
        return generate<int64_t, int32_t>(&value, structure, mlir::Type::BIGINTEGER);
    } else if (structure.getType() == mlir::Type::BIGINTEGER) {
        return generate<int64_t, int64_t>(&value, structure, mlir::Type::BIGINTEGER);
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}

template<>
lingodb::runtime::VarLen32 Array::fill(float &value, Array &structure) {
    if (structure.getNumberElements() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == mlir::Type::INTEGER) {
        return generate<float, int32_t>(&value, structure, mlir::Type::FLOAT);
    } else if (structure.getType() == mlir::Type::BIGINTEGER) {
        return generate<float, int64_t>(&value, structure, mlir::Type::FLOAT);
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}

template<>
lingodb::runtime::VarLen32 Array::fill(double &value, Array &structure) {
    if (structure.getNumberElements() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == mlir::Type::INTEGER) {
        return generate<double, int32_t>(&value, structure, mlir::Type::DOUBLE);
    } else if (structure.getType() == mlir::Type::BIGINTEGER) {
        return generate<double, int64_t>(&value, structure, mlir::Type::DOUBLE);
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}

template<>
lingodb::runtime::VarLen32 Array::fill(std::string &value, Array &structure) {
    if (structure.getNumberElements() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == mlir::Type::INTEGER) {
        return generate<char, int32_t>(value.data(), structure, mlir::Type::STRING, value.size());
    } else if (structure.getType() == mlir::Type::BIGINTEGER) {
        return generate<char, int64_t>(value.data(), structure, mlir::Type::STRING, value.size());
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}