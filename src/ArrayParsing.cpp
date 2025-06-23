#include "../include/Array.h"
#include <iostream>

void Array::fromString(std::string &source, std::string &target, mlir::Type type) {
    uint32_t dimension = 0;
    uint32_t dimensionsCounter = 0;
    uint32_t elementCounter = 0;
    uint32_t metadataIndex = 0;
    std::vector<std::string> elements;
    std::vector<uint32_t> stringLengths;
    std::vector<uint32_t> metadataLengths;
    std::vector<std::pair<uint32_t, uint32_t>> metadata;
    std::vector<bool> nulls;
    for(auto symbol = source.begin(); symbol != source.end(); symbol++) {
        switch(*symbol) {
            case '{':
            {
                auto insertPosition = metadata.begin();
                for (size_t i = 0; i < metadataLengths.size(); i++) {
                    if (i > dimension) {
                        break;
                    }
                    insertPosition += metadataLengths[i];
                }
                auto insert = metadata.insert(insertPosition, std::make_pair(elementCounter, 0));
                metadataIndex = insert - metadata.begin();
                dimension++;
                if (dimension > dimensionsCounter) {
                    metadataLengths.push_back(1);
                } else {
                    metadataLengths[dimension - 1] += 1;
                }
                dimensionsCounter = std::max(dimension, dimensionsCounter);
                break;
            }
            case '}':
            {
                metadata[metadataIndex].second = elementCounter - metadata[metadataIndex].first;
                dimension--;
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
            case ' ':
            case '\\':
            case '\t':
            case '\n':
            case '\r':
                break;
            default:
            {
                auto end = symbol;
                if (*symbol == '"') {
                    symbol++;
                    end++;
                }
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
                elements.push_back(std::string(symbol, end));
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
    elementCounter -= std::count(nulls.begin(), nulls.end(), true);
    size_t totalStringSize = 0;
    for (auto &length : stringLengths) {
        totalStringSize += length;
    }
    target.resize(getStringSize(dimensionsCounter, elementCounter, metadata.size(), nulls.size(), totalStringSize, type));
    char *writer = target.data();
    
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