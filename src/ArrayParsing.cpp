#include "../include/Array.h"
#include <iostream>

using lingodb::runtime::Array;

void Array::fromString(std::string &source, std::string &target, mlir::Type type) {
    // Stores the dimension in which the parsing is
    uint32_t dimension = 0;
    // Stores the overall dimensions of this array
    uint32_t dimensionsCounter = 0;
    // Stores the overall number of elements (with NULL values)
    uint32_t elementCounter = 0;
    // Index to know which metadata entry should be updated
    uint32_t metadataIndex = 0;
    // Stores array elements
    std::vector<std::string> elements;
    // Stores string lengths (if string is actual array type)
    std::vector<uint32_t> stringLengths;
    // Stores metadata lengths for each dimension
    std::vector<uint32_t> metadataLengths;
    // Stores metadata information
    std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> metadata;
    // Stores null values
    std::vector<bool> nulls;
    // Iterate over each character from the given string
    for(auto symbol = source.begin(); symbol != source.end(); symbol++) {
        switch(*symbol) {
            // Enter next lower dimension
            case '{':
            {
                // If not in first dimension, update dimension-length of upper dimension
                if (metadata.size() != 0) {
                    std::get<2>(metadata[metadataIndex]) += 1;
                }
                // Determine the position to add a new pair for metadata
                auto insertPosition = metadata.begin();
                for (size_t i = 0; i < metadataLengths.size(); i++) {
                    if (i > dimension) {
                        break;
                    }
                    insertPosition += metadataLengths[i];
                }
                // Add new metadata entry with known element-offset
                auto insert = metadata.insert(insertPosition, std::make_tuple(elementCounter, 0, 0));
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
                // Update metadata entry by setting element-length
                std::get<1>(metadata[metadataIndex]) = elementCounter - std::get<0>(metadata[metadataIndex]);
                dimension--;
                // Determine the index of the last metadata entry that corresponds to the upper dimension
                auto modifyPosition = metadata.begin();
                for (size_t i = 0; i < metadataLengths.size(); i++) {
                    if (i + 1 > dimension) {
                        break;
                    }
                    modifyPosition += metadataLengths[i];
                }
                if (modifyPosition != metadata.begin()) {
                    modifyPosition--;
                    metadataIndex = modifyPosition - metadata.begin();
                }
                break;
            }
            // Only check if syntax is correct
            case ',':
            {
                while(symbol < source.end() && isspace(*(symbol + 1))) {
                    symbol++;
                }
                if (symbol + 1 < source.end() && (*(symbol + 1) == ',' || *(symbol + 1) == '}')) {
                    throw std::runtime_error("Invalid array specification: Syntax error near " + std::string(symbol, symbol + 2));
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
                // Check if all elements are at the same dimension level
                if (dimensionsCounter != dimension) {
                    throw std::runtime_error("Invalid array specification: Inconsistent dimensions - found elements in different dimensions");
                }
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
    target.resize(getStringSize(dimensionsCounter, elementCounter, metadata.size(), std::ceil((double) nulls.size() / 8), totalStringSize, type));
    char *writer = target.data();
    
    // Now copy each information into the target string
    memcpy(writer, &dimensionsCounter, sizeof(uint32_t));
    writer += sizeof(uint32_t);
    memcpy(writer, &elementCounter, sizeof(uint32_t));
    writer += sizeof(uint32_t);
    memcpy(writer, metadataLengths.data(), metadataLengths.size() * sizeof(uint32_t));
    writer += metadataLengths.size() * sizeof(uint32_t);
    for (size_t i = 0; i < metadata.size(); i++) {
        memcpy(writer, &std::get<0>(metadata[i]), sizeof(uint32_t));
        writer += sizeof(uint32_t);
        memcpy(writer, &std::get<1>(metadata[i]), sizeof(uint32_t));
        writer += sizeof(uint32_t);
        memcpy(writer, &std::get<2>(metadata[i]), sizeof(uint32_t));
        writer += sizeof(uint32_t);
    }

    if (type == mlir::Type::STRING) {
        for (auto &length : stringLengths) {
            memcpy(writer, &length, sizeof(uint32_t));
            writer += sizeof(uint32_t);
        }
    } else {
        for (auto &element : elements) {
            if (type == mlir::Type::INTEGER) {
                castAndCopyElement<int32_t>(writer, element);
            } else if (type == mlir::Type::BIGINTEGER) {
                castAndCopyElement<int64_t>(writer, element);
            } else if (type == mlir::Type::FLOAT) {
                castAndCopyElement<float>(writer, element);
            } else if (type == mlir::Type::DOUBLE) {
                castAndCopyElement<double>(writer, element);
            } else {
                throw std::runtime_error("Given type is not supported in arrays");
            }
        }
    }

    copyNulls(writer, nulls);

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
        std::cout << "{" << std::get<0>(entry) << ":" << std::get<1>(entry) << ":" << std::get<2>(entry) << "}";
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

