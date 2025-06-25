#include "../include/Array.h"

std::string Array::print(mlir::Type type) {
    std::string result = "";
    transform(result, this->metadata[0], this->metadata[1], this->metadata[2], 0, 1, type);
    return result;
}

void Array::transform(std::string &target, uint32_t elemOffset, uint32_t elemLength, uint32_t dimLength, uint32_t childNumber, uint32_t dimension, mlir::Type type) {
    // Reached last dimension
    if (dimension == this->numberDimensions) {
        target.append("{");
        // Proof if array element is empty
        if (elemLength != 0) {
            // Iterate over each element and add it to result
            for (size_t i = elemOffset; i < elemOffset + elemLength; i++) {
                if (checkNull(i)) {
                    target.append("null");
                } else {
                    if (type == mlir::Type::INTEGER) {
                        target.append(toString<int32_t>(getElementPosition(i)));
                    } else if (type == mlir::Type::BIGINTEGER) {
                        target.append(toString<int64_t>(getElementPosition(i)));
                    } else if (type == mlir::Type::FLOAT) {
                        target.append(toString<float>(getElementPosition(i)));
                    } else if (type == mlir::Type::DOUBLE) {
                        target.append(toString<double>(getElementPosition(i)));
                    } else {
                        appendStringValue(target, getElementPosition(i));
                    }
                }
                if (i + 1 != elemOffset + elemLength) {
                    target.append(",");
                }
            }
        }
        target.append("}");
        return;
    }
    // Get a pointer to the first metadata entry which belongs to the next lower dimension
    auto *start = getFirstChild(dimension, childNumber);
    target.append("{");
    // Check if current metadata entry has elements
    if (dimLength != 0) {
        // Iterate over each metadata entry belonging to the caller structure.
        for (size_t i = 0; i < dimLength * 3; i += 3) {
            // Check if first element is a null value
            if (i == 0) {
                if(start[i] != elemOffset && checkNull(elemOffset)) {
                    target.append("null");
                    target.append(",");
                }
            // Check if previous element is a null value
            } else {
                if (start[i] > start[i-3] + start[i-2] && checkNull(start[i-3] + start[i-2])) {
                    target.append("null");
                    target.append(",");
                }
            }
            // Call function with its children array structure
            transform(target, start[i], start[i+1], start[i+2], getChildNumber(start + i, dimension + 1), dimension + 1, type);
            target.append(",");

            // Check if last element is null value
            if (i + 3 == dimLength * 3) {
                if (start[i] + start[i+1] < elemOffset + elemLength && checkNull(elemOffset + elemLength - 1)) {
                    target.append("null");
                    target.append(",");
                }
            }
        }
    // Check if there is a null value 
    } else if (dimLength == 0 && elemLength == 1) {
        if (checkNull(elemOffset)) {
            target.append("null");
        }
    }
    if (dimLength != 0) {
        target.erase(target.length() - 1);
    }
    target.append("}");
}