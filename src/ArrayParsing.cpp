#include "../include/Array.h"
#include <iostream>

using lingodb::runtime::Array;

void Array::fromString(std::string &source, std::string &target, mlir::Type type) {
    uint32_t dimension = 0;
    // How many characters should be ignored (defining possible header)
    uint32_t start = 0;
    std::vector<int32_t> indices;
    std::vector<int32_t> lengths;
    start = parseHeader(source, indices, lengths);
    dimension = indices.size();

    if (source.begin() + start == source.end()) {
        throw std::runtime_error("Array-Cast: Invalid array specification");
    }

    uint32_t position = 0;
    // Stores the dimension in which the elements are stored
    uint32_t lastDimension = dimension;
    // Index to know which metadata entry should be updated
    uint32_t metadataIndex = 0;
    uint32_t offset = 0;
    std::vector<std::string> elements;
    std::vector<uint32_t> stringLengths;
    std::vector<uint32_t> metadataLengths;
    std::vector<uint32_t> metadata;
    std::vector<bool> nulls;
    // Iterate over each character from the given string
    for(auto symbol = source.begin() + start; symbol != source.end(); symbol++) {
        switch(*symbol) {
            // Enter next lower dimension
            case '{':
            {
                // Restrict possiblity of infinite number of empty arrays
                if (lastDimension != 0 && lastDimension < position+1) {
                    throw std::runtime_error("Array-Cast: Invalid structure, elements are not in lowest dimension");
                }
                // If not in first dimension, update dimension-length of upper dimension
                if (metadata.size() != 0) {
                    metadata[metadataIndex] += 1;
                    // Check if new structure is inside defined bounds (Only if header is defined)
                    if (lengths.size() != 0) {
                        if (metadata[metadataIndex] > lengths[position-1]) {
                            throw std::runtime_error("Array-Cast: Invalid structure, array object is out of bounds");
                        }
                    }
                }
                // Determine the position to add a new pair for metadata
                auto insertPosition = metadata.begin();
                for (size_t i = 0; i < metadataLengths.size(); i++) {
                    if (i > position) break;
                    insertPosition += metadataLengths[i];
                }
                position++;
                // Add new metadata entry with known element-offset
                insertPosition = metadata.insert(insertPosition, 0);
                metadataIndex = insertPosition - metadata.begin();
                // Add new length information if completely new dimension has been discovered
                if (position > metadataLengths.size()) {
                    metadataLengths.push_back(1);
                // Otherwise increase length of already existing one
                } else {
                    metadataLengths[position - 1] += 1;
                }
                // Add start index if not already defined in a header
                if (position > indices.size()) {
                    indices.push_back(1);
                }
                // Update absolute number of dimensions
                dimension = std::max(dimension, position);
                offset = nulls.size();
                break;
            }
            // Enter previous upper dimension
            case '}':
            {
                // Update metadata entry only of last dimension by setting its length
                if (lastDimension == position) {
                    metadata[metadataIndex] = nulls.size() - offset;
                    // Check if new structure is inside defined bounds (Only if header is defined)
                    if (lengths.size() != 0) {
                        if (metadata[metadataIndex] > lengths[position-1]) {
                            throw std::runtime_error("Array-Cast: Invalid structure, array object is out of bounds"); 
                        }
                    }
                }
                position--;
                // Determine the index of the last metadata entry that corresponds to the upper dimension
                auto modifyPosition = metadata.begin();
                for (size_t i = 0; i < metadataLengths.size(); i++) {
                    if (i == position) break;
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
                    throw std::runtime_error("Array-Cast: Syntax error");
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
                        if (lastDimension == 0) {
                            lastDimension = position;
                        }
                        if (lastDimension != position) {
                            throw std::runtime_error("Array-Cast: NULL can only be used for primitive elements");
                        }
                        symbol += 3;
                        nulls.push_back(true);
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
                // Check if no other characters are defined outside the array
                if (position == 0) {
                    throw std::runtime_error("Array-Cast: Invalid array specification");
                }
                // Check if all elements are at the same dimension level
                if (lastDimension != 0 && lastDimension != position) {
                    throw std::runtime_error("Array-Cast: Inconsistent dimensions - found elements in different dimensions");
                }
                lastDimension = position;
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
                break;
            }
        }
    }
    // Get total size of all string values (if type is STRING)
    size_t totalStringSize = 0;
    for (auto &length : stringLengths) {
        totalStringSize += length;
    }
    // Set new size of target string
    auto size = getStringSize(dimension, elements.size(), metadata.size(), getNullBytes(nulls.size()), totalStringSize, type);
    target.resize(size);
    char *buffer = target.data();
    
    // Now copy each information into the target string
    writeToBuffer(buffer, &dimension, 1);
    uint32_t elementSize = elements.size();
    writeToBuffer(buffer, &elementSize, 1);
    writeToBuffer(buffer, indices.data(), indices.size());
    writeToBuffer(buffer, metadataLengths.data(), metadataLengths.size());
    writeToBuffer(buffer, metadata.data(), metadata.size());

    if (type == mlir::Type::STRING) {
        writeToBuffer(buffer, stringLengths.data(), stringLengths.size());
    } else {
        for (auto &element : elements) {
            if (type == mlir::Type::INTEGER) {
                castAndCopyElement<int32_t>(buffer, element);
            } else if (type == mlir::Type::BIGINTEGER) {
                castAndCopyElement<int64_t>(buffer, element);
            } else if (type == mlir::Type::FLOAT) {
                castAndCopyElement<float>(buffer, element);
            } else if (type == mlir::Type::DOUBLE) {
                castAndCopyElement<double>(buffer, element);
            } else {
                throw std::runtime_error("Given type is not supported in arrays");
            }
        }
    }

    copyNulls(buffer, nulls);

    if (type == mlir::Type::STRING) {
        for (auto &element : elements) {
            writeToBuffer(buffer, element.data(), element.length());
        }
    }
}

uint32_t Array::parseHeader(std::string &array, std::vector<int32_t> &indices, std::vector<int32_t> &lengths) {
    auto symbol = array.begin();
    bool foundHeader = false;
    try {
        // Iterate over each character
        while (symbol < array.end() && *symbol != '=' && *symbol != '{') {
            switch (*symbol) {
                // First character should be a '['
                case '[':
                {
                    foundHeader = true;
                    // Move to the first header entry value
                    symbol++;
                    while (symbol < array.end() && isspace(*symbol)) symbol++;
                    std::string value(1, *symbol);
                    symbol++;
                    // Identify all characters of that number (Must be a number)
                    while (symbol < array.end() && *symbol != ':') {
                        if (*symbol != ':' && !isdigit(*symbol) && !isspace(*symbol)) {
                            throw std::runtime_error("Array-Cast: Invalid array header");
                        }
                        value += *symbol;
                        symbol++;
                    }
                    symbol++;
                    // Add it to the indices list
                    indices.push_back(std::stoi(value));
                    // Move to the second header entry value
                    while (symbol < array.end() && isspace(*symbol)) symbol++;
                    value = std::string(1, *symbol);
                    symbol++;
                    // Identify all characters of that number (Must be a number)
                    while (symbol < array.end() && *symbol != ']') {
                        if (*symbol != ']' && !isdigit(*symbol) && !isspace(*symbol)) {
                            throw std::runtime_error("Array-Cast: Invalid array header");
                        }
                        value += *symbol;
                        symbol++;
                    }
                    symbol++;
                    // Calculate the total length of this dimension
                    int32_t lower = indices.back();
                    int32_t upper = std::stoi(value);
                    if (lower > upper) {
                        throw std::runtime_error("Array-Cast: Invalid Indices in array header");
                    }
                    if (lower > upper) std::swap(lower, upper);
                    lengths.push_back(upper - lower + 1);
                    // Go further to the next header entry / end
                    while (symbol < array.end() && isspace(*symbol)) symbol++;
                    break;
                }
                // Ignore this symbol
                case ' ':
                    symbol++;
                    break;
                default:
                    if (foundHeader) {
                        throw std::runtime_error("Array-Cast: Invalid array header");
                    } else {
                        throw std::runtime_error("Array-Cast: Invalid array specification");
                    }
            }
        }
    } catch (std::out_of_range exc) {
        throw std::runtime_error("Array-Cast: Specified index is out of range for a 32-bit integer");
    }
    if (*symbol == '=') {
        symbol++;
    }
    return symbol - array.begin();
}

