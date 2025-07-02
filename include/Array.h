#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <tuple>
#include "../include/VarLen32.h"
#include "../include/Types.h" 

namespace lingodb::runtime {

/**
 * This class represents an array structure in LingoDB.
 * @property numberDimensions   - Defines the number of dimensions in this array.
 * @property numberElements     - Defines the number of elements in this array (without NULL values).
 * @property metadataLengths    - A pointer to the size of the metadata for each dimension.
 * @property metadata           - A pointer to the first metadata value.
 * @property elements           - A pointer to the first element of this array.
 * @property nulls              - A pointer to the first bitstring, representing null values.
 * @property strings            - A pointer to the first character of a string (if array stores strings). 
 * 
 * Definition of metadata attribute:
 * This pointer points to a 'list' of integer elements which needed to be interpreted as triples. Each
 * triple defines the structure of a single array-element (opened with '{' and closed with '}'). The 
 * interpretation of each single value is as follows:
 * 1. element-offset: Index of the first element in this array structure 
 *                    (IMPORTANT: This will include NULL values).
 * 2. element-length: The number of elements inside this array structure
 *                    (IMPORTANT: This will include NULL values).
 * 3. dimension-length: The number of triples (from metadata) that corresponds to this array structure.
 *                     These triples are the metadata of the next lower dimension.
 * 
 * @example - '{{{1,2,3},{1,null,3}},{{1,2,3},{1,2,3}}}'
 * numberDimensions = 3
 * numberElements = 11
 * metadataLengths = 1,2,4 (1.Dim, 2.Dim, 3.Dim)
 * metadata = {0,12,2},{0,6,2},{6,6,2},{0,3,0},{3,3,0},{6,3,0},{9,3,0} 
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
    mlir::Type type;

/*
 *##########################################################################################################################################################  
 *                                                              PRIVATE METHODS
 *##########################################################################################################################################################
 */    

    /**
     * This function copies the specified data into the provided target.
     * 
     * @param buffer        A reference to a char pointer which points to the string
     *                      that should store the result.
     * @param data          A pointer to the data that should be copied.
     * @param size          The number of elements that should be copied from the given
     *                      data pointer.
     */
    template<class TYPE>
    static void writeToBuffer(char *&buffer, const TYPE *data, uint32_t size) {
        memcpy(buffer, data, sizeof(TYPE) * size);
        buffer += sizeof(TYPE) * size;
    };

    void printData();
    void printNulls();

    /**
     * This function casts a string into a value of the corresponding type `TYPE` and
     * copy its value to the given pointer.
     * 
     * @param value     A reference to the string which should be casted.
     * @param writer    A reference to the pointer in which the casted element should be copied.
     * @throws          `std::runtime_error`: If the value could not be casted or if the resulting
     *                  value is out of range.
     */
    template<class TYPE>
    static void castElement(std::string &value, char *&writer);

    /**
     * This function transforms an vector of booleans into a bitstring (Each element has 8-bit) and
     * copy the result to the given pointer.
     * 
     * @param nulls     A reference to the boolean vector.
     * @param writer    A reference to the pointer in which the result should be copied.
     */
    static void castNulls(std::vector<bool> &nulls, char *&writer);

    /**
     * This function calculates the necessary size of a string to store all data of the array.
     * 
     * @param dimensions        The number of dimensions.
     * @param numberElements    The number of elements (without NULL).
     * @param metadataSize      The number of metadata elements (number of triples).
     * @param nullSize          The number of bitstrings, representing NULL values.
     * @param stringSize        The total length of all string elements (if array stores string values).
     * @param type              The type of each array element.
     * @throws                  `std::runtime_error`: If the provided type is not supported.
     */
    static size_t getStringSize(uint32_t dimensions, uint32_t numberElements, uint32_t metadataSize, uint32_t nullSize, uint32_t stringSize, mlir::Type type);
    
    /**
     * This function returns the size of a specific element type.
     * 
     * @param type      The type of each array element.
     * 
     * @returns         The size of a single element of the provided type.
     */
    static size_t getTypeSize(mlir::Type type);

    uint32_t getChildNumber(const uint32_t *element, uint32_t dimension);
    uint32_t getElementPosition(uint32_t position);

    uint32_t metadataSlice(
        std::vector<uint32_t> &metadata, 
        std::vector<uint32_t> &lengths, 
        std::vector<uint32_t> &elements, 
        uint32_t lowerBound, 
        uint32_t upperBound, 
        uint32_t sliceDimension, 
        uint32_t dimension, 
        uint32_t offset, 
        const uint32_t *&entry);

    /**
     * This method casts an element at the given position to a string.
     * 
     * @param position  The position of the array element.
     * 
     * @returns         The casted element as string.
     */
    template<class TYPE>
    std::string toString(uint32_t position);

    /**
     * This method appends the array element at the given position to a string.
     * This method should only be used if the array stores string values.
     * 
     * @param target    A reference to a string in which the value should be appended.
     * @param position  The position of the array element.  
     */
    void appendStringValue(std::string &target, uint32_t position);

    /**
     * This method appends a single element of type `TYPE` to the last array structure
     * (lowest dimension). This method should only be used to append primitive elements.
     * 
     * @param value     The value which should be appended.
     * 
     * @returns         The extended array in array processable string format.
     */
    template<class TYPE>
    VarLen32 appendElement(TYPE value) {
        std::string result = "";
        size_t size = getStringSize(
            this->numberDimensions, 
            this->numberElements + 1,
            getMetadataLength(),
            countNullBytes(getTotalNumberElements() + 1),
            0,
            type
        );
        result.resize(size);
        char *buffer = result.data();

        uint32_t numberElements = this->numberElements + 1;
        writeToBuffer(buffer, &this->numberDimensions, 1);
        writeToBuffer(buffer, &numberElements, 1);
        writeToBuffer(buffer, this->metadataLengths, this->numberDimensions);
        auto *test = reinterpret_cast<uint32_t *>(buffer);
        for (size_t i = 1; i <= this->numberDimensions; i++) {
            const uint32_t *metadata = getFirstEntry(i);
            uint32_t metadataLength = getMetadataLength(i);
            uint32_t newLength = metadata[metadataLength * 3 - 2] + 1;
            writeToBuffer(buffer, metadata, (metadataLength - 1) * 3);
            writeToBuffer(buffer, &metadata[metadataLength * 3 - 3], 1);
            writeToBuffer(buffer, &newLength, 1);
            writeToBuffer(buffer, &metadata[metadataLength * 3 - 1], 1);
        }
        copyElements(buffer);
        writeToBuffer(buffer, &value, 1);
        writeToBuffer(buffer, this->nulls, countNullBytes(this->metadata[1]));
        return VarLen32::fromString(result);
    };

    /**
     * This method transforms the array into its string representation (for printing).
     * This method will be called recursively over each metadata entry.
     * 
     * @param target        A reference to a string in which the result should be stored.
     * @param elemOffset    The first element of the current array structure.
     * @param elemLength    The number of elements in this current array structure.
     * @param dimLength     The number of metadata entries of next lower dimension
     *                      corresponding to this array structure.
     * @param childNumber   The number of this metadata entry in the complete list of
     *                      all metadata entries from the same dimension.
     * @param dimension     The current dimension.
     * @param type          The type of each array element.
     * @throws              `std::runtime_error`: If the provided array structure is
     *                      not valid (processable).
     */
    void transform(std::string &target, const uint32_t *entry, uint32_t dimension);

    /**
     * This method proofs if at the given position is a NULL value.
     * 
     * @param position  The position of the array element.
     * 
     * @returns         `True` if there is a NULL value, otherwise `False`.
     */
    bool checkNull(uint32_t position);

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
    static uint32_t countNullBytes(uint32_t numberElements);

    static VarLen32 createEmptyArray(mlir::Type type);

    void copyElements(char *&buffer);
    void copyElement(char *&buffer, uint32_t position);
    void copyStrings(char *&buffer);
    void copyString(char *&buffer, uint32_t position);
    void copyNulls(char *&buffer, const uint8_t *nulls, uint32_t numberElements, uint32_t position);

    /**
     * This function counts the number of NULL values till the given position.
     * 
     * @param maxPosition   The limit up to which position should be counted.
     * 
     * @returns             The number of NULL values.
     */
    uint32_t countNulls(uint32_t maxPosition);

    const mlir::Type getType();

    /**
     * This method returns the dimension value of this array.
     */
    uint32_t getDimension();

    /**
     * This method returns the number of elements in this array without 
     * NULL values.
     */
    uint32_t getNumberElements();

    /**
     * This method returns the number of elements in this array including
     * NULL values.
     */
    uint32_t getTotalNumberElements();

    /**
     * This method returns the number of metadata entries in this array.
     */
    uint32_t getMetadataLength();

    /**
     * This method returns the number of metadata entris of a specific
     * dimension.
     * 
     * @param dimension     The selected dimension.
     */
    uint32_t getMetadataLength(uint32_t dimension);

    /**
     * This method returns the total string length of all strings in
     * this array. If the array type is not string, this method will return `0`. 
     */
    uint32_t getStringLength();

    uint32_t getStringLength(uint32_t position);

    /**
     * This method returns a pointer to the metadata entries.
     */
    const uint32_t* getMetadata();

    /**
     * This method returns a pointer to the null bitstrings.
     */
    const uint8_t* getNulls();

    /**
     * This method returns a pointer to the first metadata entry that
     * belongs to the provided dimension (Index of first dimension is
     * 1).
     * 
     * @param dimension     The selected dimension.
     * @throws              `std::runtime_error`: If the selected dimension is
     *                      larger than the total number of dimensions.
     */
    const uint32_t* getFirstEntry(uint32_t dimension);

    /**
     * This method returns a pointer to the first metadata entry that
     * belongs as child to the given metadata entry.
     * 
     * @param entry         The metadata entry.
     * @param dimension     The dimension of the given metadata entry.
     * @throws              `std::runtime_error`: If the selected dimension
     *                      is larger than the total number of dimensions.
     * 
     * @returns             A pointer to the first child metadata entry. If
     *                      not any child could be found it will return a 
     *                      `nullptr`.
     */
    const uint32_t* getChildEntry(const uint32_t *entry, uint32_t dimension);

/*
 *##########################################################################################################################################################  
 *                                                              APPEND METHODS
 *##########################################################################################################################################################
 */ 

    /**
     * This function appends the other array. Thereby a new array object will be
     * created returned as `VarLen32` object.
     * 
     * @param other     A reference to the array that should be appended.
     * @throws          `std::runtime_error`: If the array to be appended has more dimensions.
     *                  If both arrays have different types.
     * 
     * @returns         The string in array processable format storing the extended array. 
     */
    VarLen32 append(Array &other);

    /**
     * This method appends a single integer value to the last array element in the lowest 
     * dimension.
     * 
     * @param value     The value which should be appended.
     * @throws          `std::runtime_error`: If the array does not store 32-bit integer values.
     * 
     * @returns         The extended array in array processable string format.
     */
    VarLen32 append(int32_t value);

    /**
     * This method appends a single integer value to the last array element in the lowest 
     * dimension.
     * 
     * @param value     The value which should be appended.
     * @throws          `std::runtime_error`: If the array does not store 64-bit integer values.
     * 
     * @returns         The extended array in array processable string format.
     */
    VarLen32 append(int64_t value);

    /**
     * This method appends a single float value to the last array element in the lowest 
     * dimension.
     * 
     * @param value     The value which should be appended.
     * @throws          `std::runtime_error`: If the array does not store float values.
     * 
     * @returns         The extended array in array processable string format.
     */
    VarLen32 append(float value);

    /**
     * This method appends a single double value to the last array element in the lowest 
     * dimension.
     * 
     * @param value     The value which should be appended.
     * @throws          `std::runtime_error`: If the array does not store double values.
     * 
     * @returns         The extended array in array processable string format.
     */
    VarLen32 append(double value);

    /**
     * This method appends a single string value to the last array element in the lowest 
     * dimension.
     * 
     * @param value     The value which should be appended.
     * @throws          `std::runtime_error`: If the array does not store string values.
     * 
     * @returns         The extended array in array processable string format.
     */
    VarLen32 append(std::string &value);

    VarLen32 slice(uint32_t lowerBound, uint32_t upperBound, uint32_t dimension);

    std::string print();

};

}