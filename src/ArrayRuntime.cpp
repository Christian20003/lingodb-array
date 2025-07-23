#include "../include/ArrayRuntime.h"

using lingodb::runtime::ArrayRuntime;

lingodb::runtime::VarLen32 ArrayRuntime::fromString(lingodb::runtime::VarLen32 str, int32_t type) {
    std::string content = str.str();
    return lingodb::runtime::Array::fromString(content, type);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(
    lingodb::runtime::VarLen32 left,
    lingodb::runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray.append(rightArray);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, int32_t type, int32_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, int32_t type, int64_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, int32_t type, float value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, int32_t type, double value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, int32_t type, VarLen32 value) {
    std::string arrayVal = array.str();
    std::string valueVal = value.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append(valueVal);
}

lingodb::runtime::VarLen32 ArrayRuntime::append(lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append();
}

lingodb::runtime::VarLen32 ArrayRuntime::slice(lingodb::runtime::VarLen32 array, int32_t type, int32_t lowerBound, int32_t upperBound, int32_t dimension) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.slice(lowerBound, upperBound, dimension);
}

lingodb::runtime::VarLen32 ArrayRuntime::subscript(lingodb::runtime::VarLen32 array, int32_t type, int32_t position) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj[position];
}

lingodb::runtime::VarLen32 ArrayRuntime::add(
    lingodb::runtime::VarLen32 left,
    lingodb::runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray + rightArray;
}

lingodb::runtime::VarLen32 ArrayRuntime::sub(
    lingodb::runtime::VarLen32 left,
    lingodb::runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray - rightArray;
}

lingodb::runtime::VarLen32 ArrayRuntime::mul(
    lingodb::runtime::VarLen32 left,
    lingodb::runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray * rightArray;
}

lingodb::runtime::VarLen32 ArrayRuntime::div(
    lingodb::runtime::VarLen32 left,
    lingodb::runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray / rightArray;
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarAdd(lingodb::runtime::VarLen32 array, int32_t type, int32_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarAdd(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarAdd(lingodb::runtime::VarLen32 array, int32_t type, int64_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarAdd(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarAdd(lingodb::runtime::VarLen32 array, int32_t type, float value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarAdd(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarAdd(lingodb::runtime::VarLen32 array, int32_t type, double value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarAdd(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarSub(lingodb::runtime::VarLen32 array, int32_t type, int32_t value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarSub(value, isleft);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarSub(lingodb::runtime::VarLen32 array, int32_t type, int64_t value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarSub(value, isleft);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarSub(lingodb::runtime::VarLen32 array, int32_t type, float value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarSub(value, isleft);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarSub(lingodb::runtime::VarLen32 array, int32_t type, double value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarSub(value, isleft);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarMul(lingodb::runtime::VarLen32 array, int32_t type, int32_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarMul(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarMul(lingodb::runtime::VarLen32 array, int32_t type, int64_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarMul(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarMul(lingodb::runtime::VarLen32 array, int32_t type, float value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarMul(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarMul(lingodb::runtime::VarLen32 array, int32_t type, double value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarMul(value);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarDiv(lingodb::runtime::VarLen32 array, int32_t type, int32_t value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarDiv(value, isleft);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarDiv(lingodb::runtime::VarLen32 array, int32_t type, int64_t value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarDiv(value, isleft);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarDiv(lingodb::runtime::VarLen32 array, int32_t type, float value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarDiv(value, isleft);
}

lingodb::runtime::VarLen32 ArrayRuntime::scalarDiv(lingodb::runtime::VarLen32 array, int32_t type, double value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarDiv(value, isleft);
}

lingodb::runtime::VarLen32 ArrayRuntime::fill(int32_t value, lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(value, arrayObj);
}

lingodb::runtime::VarLen32 ArrayRuntime::fill(int64_t value, lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(value, arrayObj);
}

lingodb::runtime::VarLen32 ArrayRuntime::fill(float value, lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(value, arrayObj);
}

lingodb::runtime::VarLen32 ArrayRuntime::fill(double value, lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(value, arrayObj);
}

lingodb::runtime::VarLen32 ArrayRuntime::fill(VarLen32 value, lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    std::string val = value.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(val, arrayObj);
}

lingodb::runtime::VarLen32 ArrayRuntime::fill(lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(arrayObj);
}

lingodb::runtime::VarLen32 ArrayRuntime::transpose(lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.transpose();
}

lingodb::runtime::VarLen32 ArrayRuntime::sigmoid(lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.sigmoid();
}

lingodb::runtime::VarLen32 ArrayRuntime::matrixMul(
    lingodb::runtime::VarLen32 left,
    lingodb::runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray.matrixMul(rightArray);
}

int32_t ArrayRuntime::getHighestPosition(lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.getHighestPosition();
}

lingodb::runtime::VarLen32 ArrayRuntime::cast(lingodb::runtime::VarLen32 array, int32_t srcType, int32_t dstType) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, srcType);
    return arrayObj.cast(dstType);
}

lingodb::runtime::VarLen32 ArrayRuntime::increment(lingodb::runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.increment();
}