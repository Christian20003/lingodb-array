#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cstring>
#include "../include/Types.h" 

class Array {
    private:
    uint32_t numberDimensions;
    uint32_t numberElements;
    uint32_t *metadataLengths;
    uint32_t *metadata;
    uint8_t *elements;
    uint8_t *nulls;
    char *strings;

    template<class TYPE>
    static void castElement(std::string &value, char *&writer);
    static void castNulls(std::vector<bool> &nulls, char *&writer);
    static size_t getStringSize(uint32_t dimensions, uint32_t numberElements, uint32_t metadataSize, uint32_t nullSize, uint32_t stringSize, mlir::Type type);
    static size_t getTypeSize(mlir::Type type);

    size_t getMetadataLength();
    size_t getBrackets(uint32_t position);
    
    template<class TYPE>
    std::string toString(uint32_t position);

    void appendStringValue(std::string &target, uint32_t position);

    bool checkNull(int32_t position);
    uint32_t countNulls();

    public:
    Array(std::string &array, mlir::Type type);

    static void fromString(std::string &source, std::string &target, mlir::Type type);

    std::string print(mlir::Type type);
};