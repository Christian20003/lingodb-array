#include "../include/Array.h"

using lingodb::runtime::Array;

std::string Array::print() {
    std::string result = "";
    transform(result, this->metadata, 1);
    return result;
}

void Array::transform(std::string &target, const uint32_t *entry, uint32_t dimension) {
    // Reached last dimension
    if (dimension == this->numberDimensions) {
        target.append("{");
        // Proof if array element is empty
        if (entry[1] != 0) {
            // Iterate over each element and add it to result
            for (size_t i = entry[0]; i < entry[0] + entry[1]; i++) {
                if (isNull(i)) {
                    target.append("null");
                } else {
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
                if (i + 1 != entry[0] + entry[1]) {
                    target.append(",");
                }
            }
        }
        target.append("}");
        return;
    }
    // Get a pointer to the first metadata entry which belongs to the next lower dimension
    auto *start = getChildEntry(entry, dimension);
    target.append("{");
    // Check if current metadata entry has elements
    if (entry[2] != 0) {
        // Iterate over each metadata entry belonging to the caller structure.
        for (size_t i = 0; i < entry[2] * 3; i += 3) {
            // Check if first element is a null value
            if (i == 0) {
                if(start[i] != entry[0] && isNull(entry[0])) {
                    target.append("null");
                    target.append(",");
                }
            // Check if previous element is a null value
            } else {
                if (start[i] > start[i-3] + start[i-2] && isNull(start[i-3] + start[i-2])) {
                    target.append("null");
                    target.append(",");
                }
            }
            // Call function with its children array structure
            transform(target, start + i, dimension + 1);
            target.append(",");

            // Check if last element is null value
            if (i + 3 == entry[2] * 3) {
                if (start[i] + start[i+1] < entry[0] + entry[1] && isNull(entry[0] + entry[1] - 1)) {
                    target.append("null");
                    target.append(",");
                }
            }
        }
    // Check if there is a null value 
    } else if (entry[2] == 0 && entry[1] == 1) {
        if (isNull(entry[0])) {
            target.append("null");
        }
    }
    if (entry[2] != 0) {
        target.erase(target.length() - 1);
    }
    target.append("}");
}