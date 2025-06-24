#include "../include/Array.h"

std::string Array::print(mlir::Type type) {
    std::string result = "";
    transform(result, this->metadata[0], this->metadata[1], 1, type);
    return result;
}

bool Array::transform(std::string &target, uint32_t offset, uint32_t length, uint32_t dimension, mlir::Type type) {
    if (dimension == this->numberDimensions) {
        target.append("{");
        if (length != 0) {
            size_t elementIdx = offset - countNulls(offset);
            for (size_t j = offset; j < offset + length; j++) {
                if (checkNull(j)) {
                    target.append("null");
                } else {
                    if (type == mlir::Type::INTEGER) {
                        target.append(toString<int32_t>(elementIdx));
                    } else if (type == mlir::Type::BIGINTEGER) {
                        target.append(toString<int64_t>(elementIdx));
                    } else if (type == mlir::Type::FLOAT) {
                        target.append(toString<float>(elementIdx));
                    } else if (type == mlir::Type::DOUBLE) {
                        target.append(toString<double>(elementIdx));
                    } else {
                        appendStringValue(target, elementIdx);
                    }
                    elementIdx++;
                }
                if (j + 1 != offset + length) {
                    target.append(",");
                }
            }
        }
        target.append("}");
        return true;
    }
    auto *start = this->metadata;
    for (size_t i = 0; i < (size_t) dimension; i++) {
        start += this->metadataLengths[i] * 2;
    }
    size_t metadataLimit = this->metadataLengths[dimension] * 2;
    bool printed = false;
    target.append("{");
    for (size_t i = 0; i < metadataLimit; i += 2) {
        if (start[i] < offset) {
            continue;
        }
        if (start[i] + start[i+1] <= offset + length) {
            if (transform(target, start[i], start[i+1], dimension + 1, type)) {
                printed = true;
            } else {
                if (start[i+1] == 0) {
                    target.append("}");
                } else if (checkNull(start[i])) {
                    target.erase(target.length() - 1);
                    target.append("null");
                }
            }
            target.append(",");
            printed = true;
        }
        if (start[i] + start[i+1] > offset + length) {
            break;
        }
    }
    if (printed) {
        target.erase(target.length() - 1);
        target.append("}");
    }
    return printed;
}