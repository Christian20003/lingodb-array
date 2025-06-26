#include "../include/ArrayRuntime.h"

using lingodb::runtime::ArrayRuntime;

lingodb::runtime::VarLen32 ArrayRuntime::fromString(lingodb::runtime::VarLen32 str, mlir::Type type) {
    std::string content = str.str();
    std::string result = "";
    lingodb::runtime::Array::fromString(content, result, type);
    return lingodb::runtime::VarLen32::fromString(result);
}
