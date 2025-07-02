#include "../include/ArrayRuntime.h"

using lingodb::runtime::ArrayRuntime;

lingodb::runtime::VarLen32 ArrayRuntime::fromString(lingodb::runtime::VarLen32 str, mlir::Type type) {
    std::string content = str.str();
    std::string result = "";
    lingodb::runtime::Array::fromString(content, result, type);
    return lingodb::runtime::VarLen32::fromString(result);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(
    lingodb::runtime::VarLen32 left,
    lingodb::runtime::VarLen32 right,
    mlir::Type leftType,
    mlir::Type rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray.append(rightArray);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, mlir::Type type, int32_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, mlir::Type type, int64_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, mlir::Type type, float value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, mlir::Type type, double value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, mlir::Type type, VarLen32 value) {
    std::string arrayVal = array.str();
    std::string valueVal = value.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append(valueVal);
}

lingodb::runtime::VarLen32 ArrayRuntime::slice(lingodb::runtime::VarLen32 array, mlir::Type type, int32_t lowerBound, int32_t upperBound, int32_t dimension) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.slice(lowerBound, upperBound, dimension);
}

lingodb::runtime::VarLen32 ArrayRuntime::subscript(lingodb::runtime::VarLen32 array, mlir::Type type, int32_t position) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj[position];
}
