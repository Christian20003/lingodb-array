#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include "../include/Types.h" 

class Array {
    private:
    uint32_t numberDimensions;
    uint32_t numberElements;
    int32_t *metadataLengths;
    int32_t *metadata;
    uint8_t *elements = nullptr;
    uint8_t *nulls = nullptr;
    char *strings = nullptr;

    void parseString(std::string array, std::vector<std::string> &data, mlir::Type type);
    void createNulls(std::vector<bool> &nulls);
    void createElements(std::vector<std::string> &elements, mlir::Type type);

    template<class TYPE>
    void castElements(std::vector<std::string> &original);

    public:
    Array(std::string array, mlir::Type type);
    ~Array();
};