#include "../include/Array.h"

using lingodb::runtime::Array;

std::string Array::print() {
    std::string result = "";
    printHeader(result);
    printArray(result, this->metadata, 1);
    return result;
}

void Array::printArray(std::string &target, const uint32_t *entry, uint32_t dimension) {
    // Check if last dimension is reached
    if (dimension == this->numberDimensions) {
        target.append("{");
        // Iterate over each element
        for (uint32_t i = entry[0]; i < entry[0] + entry[1]; i++) {
            // Check if it is null
            if (isNull(i)) {
                target.append("null");
            } else {
                // Add the value to the result string
                if (type == mlir::Type::INTEGER) {
                    toString<int32_t>(getElementPosition(i), target);
                } else if (type == mlir::Type::BIGINTEGER) {
                    toString<int64_t>(getElementPosition(i), target);
                } else if (type == mlir::Type::FLOAT) {
                    toString<float>(getElementPosition(i), target);
                } else if (type == mlir::Type::DOUBLE) {
                    toString<double>(getElementPosition(i), target);
                } else {
                    toString<std::string>(getElementPosition(i), target);
                }
            }
            // If not last element, add a comma
            if (i + 1 != entry[0] + entry[1]) target.append(",");
        }
        target.append("}");
        return;
    }
    // Get a pointer to the first metadata entry which belongs to the next lower dimension
    auto *children = getChildEntry(entry, dimension);
    target.append("{");
    // Iterate over each metadata entry belonging to the caller structure.
    for (uint32_t i = 0; i < entry[1]; i++) {
        // Call function with its children
        printArray(target, children + i*entrySize, dimension + 1);
        // If not last element, add a comma
        if (i+1 != entry[1]) target.append(",");        
    }
    target.append("}");
}

void Array::printHeader(std::string &target) {
    // Check if header must be printed
    auto print = false;
    for (uint32_t i = 0; i < this->numberDimensions; i++) {
        if (this->indices[i] != 1) print = true;
    }
    if (print) {
        // Iterate over each index and add lower and upper bound to result string
        for (uint32_t i = 0; i < this->numberDimensions; i++) {
            auto length = getMaxDimensionSize(i+1);
            auto expr = "[" + std::to_string(this->indices[i]) + ":" + std::to_string(this->indices[i] + length - 1) + "]";
            target.append(expr);
        }
        target.append("=");
    }
}