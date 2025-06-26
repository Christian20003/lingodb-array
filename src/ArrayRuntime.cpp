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
