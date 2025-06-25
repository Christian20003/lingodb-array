#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <tuple>
#include "../include/Types.h" 

/**
 * This class represents an array structure in LingoDB.
 * @property numberDimensions   - Defines the number of dimensions in this array.
 * @property numberElements     - Defines the number of elements in this array (without NULL values).
 * @property metadataLengths    - A pointer to the size of the metadata for each dimension.
 * @property metadata           - A pointer to the first metadata value.
 * @property elements           - A pointer to the first elements of this array.
 * @property nulls              - A pointer to the first bitstring representing null values.
 * @property strings            - A pointer to the first character of a string (if array stores strings). 
 * 
 * Metadata consists of multiple value pairs that specify the size of a dimension ({offset, length}). 
 * 
 * @example - '{{{1,2,3},{1,null,3}},{{1,2,3},{1,2,3}}}'
 * numberDimensions = 3
 * numberElements = 11
 * metadataLengths = 1,2,4 (1.Dim, 2.Dim, 3.Dim)
 * metadata = {0,12},{0,6},{6,6},{0,3},{3,3},{6,3},{9,3} 
 * elements = ...
 * nulls = 0000.1000, 0000.0000 (bitstrings - from left to right) 
 */
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

    size_t getEmptyElement(uint32_t position);
    
    template<class TYPE>
    std::string toString(uint32_t position);

    void appendStringValue(std::string &target, uint32_t position);

    void transform(std::string &target, uint32_t offset, uint32_t length, uint32_t subarray, uint32_t ignore, uint32_t dimension, mlir::Type type);

    bool checkNull(int32_t position);
    uint32_t countNulls(uint32_t maxPosition);

    public:
    /**
     * This constructor generates an array object by extracting all data from the given string.
     * The given string should be in a processible array format. Otherwise use the `fromString`
     * method.
     * @param array     A reference to the string which stores the array data.
     * @param type      The type of each array element. 
     */
    Array(std::string &array, mlir::Type type);

    /**
     * This function parses the raw string into a processible array format.
     * 
     * @param source    A reference to the source string.
     * @param target    A reference to the string that stores the result.
     * @param type      The type of each array element.
     * @throws          `std::runtime_error`: If the given string has invalid syntax or if the
     *                  provided elements could not be casted to the specified type.
     */
    static void fromString(std::string &source, std::string &target, mlir::Type type);

    std::string print(mlir::Type type);
};