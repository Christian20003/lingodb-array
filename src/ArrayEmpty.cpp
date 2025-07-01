#include "../include/Array.h"

using lingodb::runtime::Array;

lingodb::runtime::VarLen32 Array::createEmptyArray(mlir::Type type) {
    std::string result = "";
    uint32_t dimension = 1;
    uint32_t elements = 0;
    size_t size = getStringSize(1, 0, 1, 0, 0, type);
    result.resize(size);
    char *buffer = &result[0];

    writeToBuffer(buffer, &dimension, 1);
    writeToBuffer(buffer, &elements, 1);
    writeToBuffer(buffer, &dimension, 1);
    writeToBuffer(buffer, &elements, 1);
    writeToBuffer(buffer, &elements, 1);
    writeToBuffer(buffer, &elements, 1);

    return VarLen32::fromString(result);
}