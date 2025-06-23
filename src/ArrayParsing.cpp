#include "../include/Array.h"
#include <iostream>

void Array::fromString(std::string &source, std::string &target, mlir::Type type) {
    uint32_t dimension = 0;
    uint32_t dimensionsCounter = 0;
    uint32_t elementCounter = 0;
    // Index to know which metadata entry should be updated
    uint32_t metadataIndex = 0;
    std::vector<std::string> elements;
    std::vector<uint32_t> stringLengths;
    std::vector<uint32_t> metadataLengths;
    std::vector<std::pair<uint32_t, uint32_t>> metadata;
    std::vector<bool> nulls;
    // Iterate over each character from the given string
    for(auto symbol = source.begin(); symbol != source.end(); symbol++) {
        switch(*symbol) {
            // Enter next lower dimension
            case '{':
            {
                // Determine the position to add a new pair for metadata
                auto insertPosition = metadata.begin();
                for (size_t i = 0; i < metadataLengths.size(); i++) {
                    if (i > dimension) {
                        break;
                    }
                    insertPosition += metadataLengths[i];
                }
                // Add new metadata entry with known offset and unknown length
                auto insert = metadata.insert(insertPosition, std::make_pair(elementCounter, 0));
                metadataIndex = insert - metadata.begin();
                dimension++;
                // Add new length information if completely new dimension has been discovered
                if (dimension > dimensionsCounter) {
                    metadataLengths.push_back(1);
                // Otherwise increase length of already existing one
                } else {
                    metadataLengths[dimension - 1] += 1;
                }
                // Update absolute number of dimensions
                dimensionsCounter = std::max(dimension, dimensionsCounter);
                break;
            }
            // Enter previous upper dimension
            case '}':
            {
                // Update metadata entry by setting its length
                metadata[metadataIndex].second = elementCounter - metadata[metadataIndex].first;
                dimension--;
                // Determine the index of the last metadata entry that corresponds to the upper dimension
                auto modifyPosition = metadata.begin();
                for (size_t i = 0; i < metadataLengths.size(); i++) {
                    if (i + 1 > dimension) {
                        break;
                    }
                    modifyPosition += metadataLengths[i];
                }
                modifyPosition--;
                metadataIndex = modifyPosition - metadata.begin();
                break;
            }
            // Only check if syntax is correct
            case ',':
            {
                while(symbol < source.end() && isspace(*(symbol + 1))) {
                    symbol++;
                }
                if (symbol + 1 < source.end() && (*(symbol + 1) == ',' || *(symbol + 1) == '}')) {
                    throw std::runtime_error("Invalid array specification");
                }
                break;
            }
            // Check if NULL value has been discovered
            case 'n':
            case 'N':
            {
                if (symbol + 4 < source.end()) {
                    char possibleU = *(symbol + 1);
                    char possibleL1 = *(symbol + 2);
                    char possibleL2 = *(symbol + 3);
                    if ((possibleU == 'u' || possibleU == 'U') && (possibleL1 == 'l' || possibleL1 == 'L') && (possibleL2 == 'l' || possibleL2 == 'L')) {
                        nulls.push_back(true);
                        elementCounter++;
                        symbol += 3;
                    }
                }
                break;
            }
            // Ignore these special characters
            case ' ':
            case '\\':
            case '\t':
            case '\n':
            case '\r':
                break;
            // Default = New array element discovered
            default:
            {
                // Jump over " if present
                auto end = symbol;
                if (*symbol == '"') {
                    symbol++;
                    end++;
                }
                // Find end of array element
                while (end < source.end()) {
                    if (*end == '"' && *(end - 1) != '\\') {
                        break;
                    }
                    if (*end == ',' && *(end - 1) != '\\') {
                        break;
                    }
                    if (*end == '}' && *(end - 1) != '\\') {
                        break;
                    }
                    end++;
                }
                // Add discovered element
                elements.push_back(std::string(symbol, end));
                // If type is STRING, also store the elements length
                if (type == mlir::Type::STRING) {
                    stringLengths.push_back(end - symbol);
                    symbol = end;
                } else {
                    symbol = end - 1;
                }
                nulls.push_back(false);
                elementCounter++;
                break;
            }
        }
    }
    // Remove number of NULL values
    elementCounter -= std::count(nulls.begin(), nulls.end(), true);
    // Get total size of all string values (if type is STRING)
    size_t totalStringSize = 0;
    for (auto &length : stringLengths) {
        totalStringSize += length;
    }
    // Set new size of target string
    target.resize(getStringSize(dimensionsCounter, elementCounter, metadata.size(), nulls.size(), totalStringSize, type));
    char *writer = target.data();
    
    // Now copy each information into the target string
    memcpy(writer, &dimensionsCounter, sizeof(uint32_t));
    writer += sizeof(uint32_t);
    memcpy(writer, &elementCounter, sizeof(uint32_t));
    writer += sizeof(uint32_t);
    memcpy(writer, metadataLengths.data(), metadataLengths.size() * sizeof(uint32_t));
    writer += metadataLengths.size() * sizeof(uint32_t);
    memcpy(writer, metadata.data(), metadata.size() * sizeof(std::pair<uint32_t, uint32_t>));
    writer += metadata.size() * sizeof(std::pair<uint32_t, uint32_t>);

    if (type == mlir::Type::STRING) {
        for (auto &length : stringLengths) {
            memcpy(writer, &length, sizeof(uint32_t));
            writer += sizeof(uint32_t);
        }
    } else {
        for (auto &element : elements) {
            if (type == mlir::Type::INTEGER) {
                castElement<int32_t>(element, writer);
            } else if (type == mlir::Type::BIGINTEGER) {
                castElement<int64_t>(element, writer);
            } else if (type == mlir::Type::FLOAT) {
                castElement<float>(element, writer);
            } else if (type == mlir::Type::DOUBLE) {
                castElement<double>(element, writer);
            } else {
                throw std::runtime_error("Given type is not supported in arrays");
            }
        }
    }

    castNulls(nulls, writer);

    if (type == mlir::Type::STRING) {
        for (auto &element : elements) {
            memcpy(writer, element.data(), element.length());
            writer += element.length();
        }
    }

    // Currently only for debugging purposes
    /* std::cout << dimensionsCounter << std::endl;
    std::cout << elementCounter << std::endl;
    for (auto &entry : metadataLengths) {
        std::cout << entry << ",";
    }
    std::cout << std::endl;
    for (auto &entry : metadata) {
        std::cout << "{" << entry.first << ":" << entry.second << "}";
    }
    std::cout << std::endl;
    for (auto &entry : elements) {
        std::cout << entry << ",";
    }
    std::cout << std::endl;
    for (auto entry : nulls) {
        std::cout << entry << ",";
    }
    std::cout << std::endl;
    for (auto entry : stringLengths) {
        std::cout << entry << ",";
    }
    std::cout << std::endl; */
}


std::string Array::print(mlir::Type type) {
    std::string result = "";
    size_t elementIdx = 0;
    for(size_t i = 0; i < this->numberElements + countNulls(); i++) {
        auto brackets = getBrackets(i);
        bool isNull = checkNull(i);
        if (i != 0 && brackets != 0) {
            result.erase(result.length() - 1);
            result.append(brackets, '}');
            result.append(",");
        }
        result.append(brackets, '{');
        if (isNull) {
            result.append("null");
        } else {
            if (type == mlir::Type::INTEGER) {
                result.append(toString<int32_t>(elementIdx));
            } else if (type == mlir::Type::BIGINTEGER) {
                result.append(toString<int64_t>(elementIdx));
            } else if (type == mlir::Type::FLOAT) {
                result.append(toString<float>(elementIdx));
            } else if (type == mlir::Type::DOUBLE) {
                result.append(toString<double>(elementIdx));
            } else {
                appendStringValue(result, elementIdx);
            }
            elementIdx++;
        }
        result.append(",");
    }
    result.erase(result.length() - 1);
    result.append(this->numberDimensions, '}');
    return result;
}