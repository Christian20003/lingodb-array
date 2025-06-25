#include "../include/Array.h"

std::string Array::print(mlir::Type type) {
    std::string result = "";
    transform(result, this->metadata[0], this->metadata[1], this->metadata[2], 0, 1, type);
    return result;
}

void Array::transform(std::string &target, uint32_t offset, uint32_t length, uint32_t subarray, uint32_t ignore, uint32_t dimension, mlir::Type type) {
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
        return;
    }
    auto *start = this->metadata;
    for (size_t i = 0; i < (size_t) dimension; i++) {
        start += this->metadataLengths[i] * 3;
    }
    uint32_t ignoreMetadata = 0;
    for (size_t i = 0; i < ignore * 3; i += 3) {
        ignoreMetadata += start[i+2];
    }
    start += ignore * 3;
    target.append("{");
    if (subarray != 0) {
        for (size_t i = 0; i < subarray * 3; i += 3) {
            if (start[i] == 11) {
                int j = 0;
            }
            if (i == 0) {
                if(start[i] != offset && checkNull(offset)) {
                    target.append("null");
                    target.append(",");
                }
            } else {
                if (start[i] > start[i-3] + start[i-2] && checkNull(start[i-3] + start[i-2])) {
                    target.append("null");
                    target.append(",");
                }
            }

            transform(target, start[i], start[i+1], start[i+2], ignoreMetadata, dimension + 1, type);
            target.append(",");

            if (i + 3 == subarray * 3) {
                if (start[i] + start[i+1] < offset + length && checkNull(offset + length - 1)) {
                    target.append("null");
                    target.append(",");
                }
            }
            ignoreMetadata += start[i+2];
        }
    } else if (subarray == 0 && length == 1) {
        if (checkNull(offset)) {
            target.append("null");
        }
    }
    if (subarray != 0) {
        target.erase(target.length() - 1);
    }
    target.append("}");
}