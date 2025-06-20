#include "../include/Array.h"

void Array::createNulls(std::vector<bool> &nulls) {
    int32_t size = std::ceil(nulls.size() / 8);
    this->nulls = new uint8_t[size];
    uint8_t value = 0; 
    size_t index = 0;
    for (size_t i = 0; i < nulls.size(); i++) {
        if (i % 8 == 0) {
            this->nulls[index] = value;
            value = 0;
            index++;
        }
        if (nulls[i]) {
            uint8_t index = i % 8;
            uint8_t shift = 8 - index - 1;
            value = value || (1 << shift);
        }
    }
}

void Array::createElements(std::vector<std::string> &elements, mlir::Type type) {
    if (type == mlir::Type::INTEGER) {
        castElements<int32_t>(elements);
    } else if (type == mlir::Type::BIGINTEGER) {
        castElements<int64_t>(elements);
    } else if (type == mlir::Type::FLOAT) {
        castElements<float>(elements);
    } else if (type == mlir::Type::DOUBLE) {
        castElements<double>(elements);
    } else if (type == mlir::Type::STRING) {
        castElements<std::string>(elements);
    } else {
        throw std::runtime_error("Specified type is not supported with arrays");
    }
}

void Array::parseString(std::string array, std::vector<std::string> &data, mlir::Type type) {
    uint32_t dimensions = 0;
    uint32_t elementCounter = 0;
    uint64_t metadata_index = 0;
    bool insideString = false;
    std::vector<int32_t> metadataLengths;
    std::vector<std::pair<int32_t, int32_t>> metadata;
    std::vector<bool> nulls;
    for(auto symbol = array.begin(); symbol != array.end(); symbol++) {
        switch(*symbol) {
            case '{':
                dimensions++;
                metadata_index = metadata_index == 0 ? 0 : metadata_index + 1;
                metadata.insert(metadata.end(), std::make_pair(elementCounter, 0));
                metadataLengths.insert(metadataLengths.end(), 1);
                break;
            case '}':
                metadata[metadata_index].second = elementCounter - metadata[metadata_index].first;
                metadata_index--;
                break;
            case ',':
                metadataLengths[metadata_index]++;
                break;
            case 'n':
            case 'N':
                if (!insideString && symbol + 4 < array.end()) {
                    char possibleU = *(symbol + 1);
                    char possibleL1 = *(symbol + 2);
                    char possibleL2 = *(symbol + 3);
                    if ((possibleU == 'u' || possibleU == 'U') && (possibleL1 == 'l' || possibleL1 == 'L') && (possibleL2 == 'l' || possibleL2 == 'L')) {
                        nulls.push_back(true);
                        symbol += 3;
                    }
                }
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                break;
            default:
                auto end = symbol;
                if (*symbol == '"') {
                    symbol++;
                    end++;
                    insideString = true;
                }
                while (end < array.end()) {
                    if (*end == '"' && *(end - 1) != '\\' && !insideString) {
                        insideString = false;
                        break;
                    }
                    if (*end == ',' && *(end - 1) != '\\' && !insideString) {
                        break;
                    }
                    if (*end == '}' && *(end - 1) != '\\' && !insideString) {
                        break;
                    }
                    end++;
                }
                data.push_back(std::string(symbol, end));
                nulls.push_back(false);
                elementCounter++;
                symbol = end;
        }
    }    
    
    this->numberDimensions = dimensions;
    this->numberElements = elementCounter;
    this->metadataLengths = new int32_t[metadataLengths.size()];
    this->metadata = new int32_t[metadata.size() * 2];
    std::copy(metadataLengths.begin(), metadataLengths.end(), this->metadataLengths);
    for (size_t i = 0; i < metadata.size(); i++) {
        this->metadata[2*i] = metadata[i].first;
        this->metadata[2*i+1] = metadata[i].second;
    }
    createNulls(nulls);
}