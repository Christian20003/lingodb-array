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
    uint8_t *elements = nullptr;
    uint8_t *nulls = nullptr;
    char *strings = nullptr;

    template<class TYPE>
    static void castElement(std::string &value, char *writer);
    static void castNulls(std::vector<bool> &nulls, char *writer);
    static size_t calculateSize(uint32_t dimensions, uint32_t numberElements, uint32_t metadataSize, uint32_t nullSize, uint32_t stringSize, mlir::Type type);
    
    template<class TYPE>
    std::string toString(int32_t position);

    bool checkNull(int32_t position);
    int32_t countNulls();

    public:
    Array(std::string array, mlir::Type type);
    ~Array();

    static void fromString(std::string &source, std::string &target, mlir::Type type);

    std::string str(mlir::Type type);
};