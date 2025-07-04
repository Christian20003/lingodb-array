#ifndef LINGODB_RUNTIME_ARRAY_H
#define LINGODB_RUNTIME_ARRAY_H

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <tuple>
#include "ArrayArithmetic.h"
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
    static void writeToBuffer(char *&buffer, const TYPE *data, uint32_t size);

    void printData();
    void printNulls();

    /**
     * This function casts a string into a value of the corresponding type `TYPE` and
     * copy its value to the given pointer.
     * 
     * @param buffer A reference to the pointer in which the casted element should be copied.
     * @param value A reference to the string which should be casted.
     * @throws `std::runtime_error`: If the value could not be casted or if the resulting
     * value is out of range.
     */
    template<class TYPE>
    static void castAndCopyElement(char *&buffer, std::string &value);

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

    /**
     * This method returns the exact element position of a value stored in the 
     * element section of an array.
     * 
     * @param position The absolute position of the element (including NULL values).
     * @throws `std::runtime_error`: If the given position is out of range.
     * @return The relative position of the element (excluding NULL values).
     */
    uint32_t getElementPosition(uint32_t position);

    /**
     * This method executes a slice operation on metadata level (This method will
     * be called recursively).
     * 
     * @param metadata      A reference to a vector that stores the updated
     *                      metadata entries.
     * @param lengths       A reference to a vector that stores the updated
     *                      metadata lengths.
     * @param elements      A reference to a vector that stores the position
     *                      of each element that should remain (including NULL
     *                      values).
     * @param lowerBound    The index of the first element that should remain.
     *                      Possible value range `[1:]`.
     * @param upperBound    The index of the last element that should remain.
     *                      Possible value range `[1:]`.
     * @param sliceDimension    The dimension in which the slice operation should take
     *                          place.
     * @param dimension     The current dimension.
     * @param offset        The adjusted offset value.
     * @param entry         A pointer to the metadata element that is processed.
     * 
     * @returns             The number of elements in the array structure.
     */
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
     * This method casts an element at the given position to a string (only if value
     * is not of type string) and append it to the end of the target string.
     * 
     * @param position The position of the array element. Possible value range 
     * `[0:numberElements-1]`.
     * @throws `std::runtime_error`: If the given position is out of range.
     */
    template<class TYPE>
    void toString(uint32_t position, std::string &target);

    /**
     * This method appends a single element of type `TYPE` to the last array structure
     * (lowest dimension). This method should only be used to append primitive elements.
     * 
     * @param value     The value which should be appended.
     * 
     * @returns         The extended array in array processable string format.
     */
    template<class TYPE>
    VarLen32 appendElement(TYPE value);

    /**
     * This method executes a specified binary scalar operation (`OP`).
     * 
     * @param value The scalar value of type `TYPE`. Should be numeric type.
     * @param isLeft If the scalar is on the left side of the operation.
     * @return The result of the binary scalar operation as string in array
     * processable format. 
     */
    template<class TYPE, class OP>
    VarLen32 executeScalarOperation(TYPE value, bool isLeft);

    /**
     * This function executes a specified binary function `OP` with numeric values.
     * 
     * @param left A pointer to the value of the first parameter of the binary function.
     * @param right A pointer to the value of the second parameter of the binary function.
     * @param size The length of both value lists.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param scalarLeft If the left parameter points to a single element.
     * @param scalarRight If the right parameter points to a single element. 
     * @param type The element type.
     * @throws `std::runtime_error`: If the array type is not supported in this function.
     */
    template<class OP>
    static void executeOperation(const uint8_t *left, const uint8_t *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight, mlir::Type type);

    /**
     * This function generates an array with the given value and structure.
     * 
     * @param value A pointer to the value which represents the array element.
     * @param structure A reference to an array which specifies the resulting
     * structure. NULL values and multiple dimensions will be ignored.
     * @param type The type of the array elements.
     * @param stringSize The length of the string element (if value is of type `char`)
     * @return The resulting array as string in array processable format.
     */
    template<class TYPE, class ARRAYTYPE>
    static VarLen32 generate(TYPE *value, Array &structure, mlir::Type type, uint32_t stringSize = 0);

    /**
     * This method transforms the array into its string representation (for printing).
     * This method will be called recursively over each metadata entry.
     * 
     * @param target        A reference to a string in which the result should be stored.
     * @param entry         A pointer to the metadata element that is processed.
     * @param dimension     The current dimension.
     * @throws              `std::runtime_error`: If the provided array structure is
     *                      not valid (processable).
     */
    void transform(std::string &target, const uint32_t *entry, uint32_t dimension);

    /**
     * This method proofs if the given position is a NULL value.
     * 
     * @param position The position of the array element.
     * @return `True` if there is a NULL value, otherwise `False`.
     */
    bool isNull(uint32_t position);

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

    static VarLen32 createEmptyArray(mlir::Type type);

    /**
     * This method copies all array elements in this array into the provided buffer.
     * 
     * @param buffer    A reference to the string buffer where the content needs to be stored.
     */
    void copyElements(char *&buffer);

    /**
     * This method copies the array element on the given position into the provided buffer.
     * 
     * @param buffer    A reference to the string buffer where the content needs to be stored.
     * @param position  The position of the element. Possible value range `[0:numberElements-1]`. 
     *                  Position values should not include NULL values.
     * @throws          `std::runtime_error`: If the position is out of range.
     */
    void copyElement(char *&buffer, uint32_t position);

    /**
     * This method copies all strings in this array into the provided buffer.
     * 
     * @param buffer    A reference to the string buffer where the content needs to be stored.
     */
    void copyStrings(char *&buffer);

    /**
     * This method copies the string on the given position into the provided buffer.
     * 
     * @param buffer    A reference to the string buffer where the content needs to be stored.
     * @param position  The position of the element. Possible value range `[0:numberElements-1]`. 
     *                  Position values should not include NULL values.
     * @throws          `std::runtime_error`: If the position is out of range.
     */
    void copyString(char *&buffer, uint32_t position);

    /**
     * This method copies the given null bits into the provided buffer.
     * 
     * @param buffer            A reference to the string buffer where the content needs to be stored.
     * @param nulls             A pointer to the null bitstrings that should be copied.
     * @param numberElements    The number of elements in this array (including NULL values).
     * @param position          The start position of the first element. 
     */
    void copyNulls(char *&buffer, const uint8_t *nulls, uint32_t numberElements, uint32_t position);

    /**
     * This function copies the boolean values from the given vector into the buffer.
     * Thereby each value is transformed into a single bit and stored in a bitstring (with size 8-bit). 
     * The order is from left to right. 
     * 
     * @param buffer A reference to the string buffer where the content needs to be stored.
     * @param nulls A reference to the boolean vector.
     */
    static void copyNulls(char *&buffer, std::vector<bool> &nulls);

    /**
     * This function counts the number of NULL values up to the specified position.
     * 
     * @param maxPosition The limit up to which position should be counted.
     * Possible value range `[0:numberElements + nulls - 1]`.
     * @throws `std::runtime_error`: If the selected position is out of range.
     * @return The number of NULL values.
     */
    uint32_t countNulls(uint32_t maxPosition);

    mlir::Type getType();

    /**
     * This method returns the dimension value of this array.
     */
    uint32_t getDimension();

    /**
     * This method returns the number of elements in this array.
     * 
     * @param withNulls     If NULL values should be included.
     */
    uint32_t getNumberElements(bool withNulls = false);

    /**
     * This method returns the number of metadata entries in this array.
     */
    uint32_t getMetadataLength();

    /**
     * This method returns the number of metadata entris of a specific
     * dimension.
     * 
     * @param dimension     The selected dimension. Possible value range
     *                      `[1:numberDimensions]`.
     * @throws              `std::runtime_error`: If the selected dimension
     *                      is out of range.
     */
    uint32_t getMetadataLength(uint32_t dimension);

    /**
     * This method returns the total string length of all strings in
     * this array. If the array type is not string, this method will return `0`. 
     */
    uint32_t getStringLength();

    /**
     * This method returns the string length of a particular element.
     * 
     * @param position      The position of the element. Possible value range
     *                      `[0:numberElements-1]`. Position values should not 
     *                      include NULL values.
     * @throws              `std::runtime_error`: If the selected position is
     *                      out of range.
     * @returns             A string length if the array stores strings, 
     *                      otherwise `0`.
     */
    uint32_t getStringLength(uint32_t position);

    /**
     * This method returns a pointer to the metadata entries.
     */
    const uint32_t* getMetadata();

    /**
     * This method returns a pointer to the array elements.
     */
    const uint8_t* getElements();

    /**
     * This method returns a pointer to the null bitstrings.
     */
    const uint8_t* getNulls();

    /**
     * This function returns the number of bytes required to store NULL bits
     * for each position.
     * 
     * @param numberElements The number of elements that requires a NULL bit.
     * @return Number of NULL bytes.
     */
    static uint32_t getNullBytes(uint32_t numberElements);

    /**
     * This method returns a pointer to the first metadata entry that
     * belongs to the provided dimension.
     * 
     * @param dimension     The selected dimension. Possible value range
     *                      `[1:numberDimensions]`.
     * @throws              `std::runtime_error`: If the selected dimension is
     *                      out of range.
     */
    const uint32_t* getFirstEntry(uint32_t dimension);

    /**
     * This method returns a pointer to the first metadata entry that
     * belongs as child to the given metadata entry.
     * 
     * @param entry         A pointer to the metadata entry.
     * @param dimension     The dimension of the given metadata entry. 
     *                      Possible value range `[1:numberDimensions]`.
     * @throws              `std::runtime_error`: If the selected dimension
     *                      is out of range.
     * @returns             A pointer to the first child metadata entry. If
     *                      not any child could be found it will return a 
     *                      `nullptr`.
     */
    const uint32_t* getChildEntry(const uint32_t *entry, uint32_t dimension);

    /**
     * This method proofs if the given metadata pointer has the same structure
     * (equal elements) as the metadata from this array.
     * 
     * @param other A pointer to the first metadata entry of another array.
     * @return `True` if each metadata entry is equal, otherwise `False`.
     */
    bool equalMetadata(const uint32_t *other);

    /**
     * This method proofs if the array contains NULL values.
     * 
     * @return `True` if a NULL value has been found, otherwise `False`.
     */
    bool hasNullValue();

    /**
     * This method proofs if the array contains an empty array structure.
     * 
     * @return `True` if an empty array structure has been found, otherwise `False`.
     */
    bool hasEmptyValue();

    /**
     * This method proofs if the array is symmetric. This means that each dimension
     * must have equal sizes.
     * 
     * @return `True` if the array is symmetric in each dimension, otherwise `False`.
     */
    bool isSymmetric();

    /**
     * This method proofs if the element type is numeric.
     * 
     * @return `True` if the element type is numeric, otherwise `False`.
     */
    bool isNumericType();

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

    /**
     * This method executes a slice operation.
     * 
     * @param lowerBound    The index of the first element that should remain.
     *                      Possible value range `[1:]`.
     * @param upperBound    The index of the last element that should remain.
     *                      Possible value range `[1:]`.
     * @param dimension     The dimension in which the slice operation should take
     *                      place.
     * @throws              `std::runtime_error`: If the given lowerBound is larger
     *                      than the given upperBound.
     * @returns             The modified array after the slice operation as string in
     *                      array processable format. If the given lowerBound and upperBound
     *                      are out of range, this method will return an empty array in the 
     *                      specified dimension.
     */
    VarLen32 slice(uint32_t lowerBound, uint32_t upperBound, uint32_t dimension);

/*
*##########################################################################################################################################################  
*                                                              OPERATOR METHODS
*##########################################################################################################################################################
*/ 

    /**
     * This method executes the subscript operator to get an element of this array.
     * 
     * @param position The index of the array element that should be returned. Possible
     * value range `[1:]`.
     * @return If the array has more than one dimension, it returns another array. If the
     * array has only a single dimension, it returns the element as string. If no
     * element could be found, it returns an empty string.
     */
    VarLen32 operator[](uint32_t position);

    /**
     * This method executes elementwise addition on each element.
     * 
     * @param other A reference to the array whose values are to be used for the addition.
     * @throws `std::runtime_error`: If the array type is not numeric. If both arrays have
     * different types. If one array has NULL values or empty array structures. If both
     * arrays have unequal array structures (unequal metadata).
     * @return The result array as string in array processable format.
     */
    VarLen32 operator+(Array &other);

    /**
     * This method executes elementwise subtraction on each element.
     * 
     * @param other A reference to the array whose values are to be used for the subtraction.
     * @throws `std::runtime_error`: If the array type is not numeric. If both arrays have
     * different types. If one array has NULL values or empty array structures. If both
     * arrays have unequal array structures (unequal metadata).
     * @return The result array as string in array processable format.
     */
    VarLen32 operator-(Array &other);

    /**
     * This method executes elementwise multiplication on each element.
     * 
     * @param other A reference to the array whose values are to be used for the multiplication.
     * @throws `std::runtime_error`: If the array type is not numeric. If both arrays have
     * different types. If one array has NULL values or empty array structures. If both
     * arrays have unequal array structures (unequal metadata).
     * @return The result array as string in array processable format.
     */
    VarLen32 operator*(Array &other);

    /**
     * This method executes elementwise division on each element.
     * 
     * @param other A reference to the array whose values are to be used for the division.
     * @throws `std::runtime_error`: If the array type is not numeric. If both arrays have
     * different types. If one array has NULL values or empty array structures. If both
     * arrays have unequal array structures (unequal metadata).
     * @return The result array as string in array processable format.
     */
    VarLen32 operator/(Array &other);

/*
*##########################################################################################################################################################  
*                                                              SCALAR METHODS
*##########################################################################################################################################################
*/ 

    /**
     * This method executes scalar addition. All elements of the array will be added
     * with the provided value.
     * 
     * @param value The scalar value.
     * @throws 'std::runtime_error': If the array does not store 32-bit integer values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarAdd(int32_t value);

    /**
     * This method executes scalar addition. All elements of the array will be added
     * with the provided value.
     * 
     * @param value The scalar value.
     * @throws 'std::runtime_error': If the array does not store 64-bit integer values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarAdd(int64_t value);

    /**
     * This method executes scalar addition. All elements of the array will be added
     * with the provided value.
     * 
     * @param value The scalar value.
     * @throws 'std::runtime_error': If the array does not store float values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarAdd(float value);

    /**
     * This method executes scalar addition. All elements of the array will be added
     * with the provided value.
     * 
     * @param value The scalar value.
     * @throws 'std::runtime_error': If the array does not store double values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarAdd(double value);

    /**
     * This method executes scalar subtraction. All elements of the array will be subtracted
     * with the provided value.
     * 
     * @param value The scalar value.
     * @param isLeft If the scalar is on the left side of the operation.
     * @throws 'std::runtime_error': If the array does not store 32-bit integer values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarSub(int32_t value, bool isLeft);

    /**
     * This method executes scalar subtraction. All elements of the array will be subtracted
     * with the provided value.
     * 
     * @param value The scalar value.
     * @param isLeft If the scalar is on the left side of the operation.
     * @throws 'std::runtime_error': If the array does not store 64-bit integer values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarSub(int64_t value, bool isLeft);

    /**
     * This method executes scalar subtraction. All elements of the array will be subtracted
     * with the provided value.
     * 
     * @param value The scalar value.
     * @param isLeft If the scalar is on the left side of the operation.
     * @throws 'std::runtime_error': If the array does not store float values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarSub(float value, bool isLeft);

    /**
     * This method executes scalar subtraction. All elements of the array will be subtracted
     * with the provided value.
     * 
     * @param value The scalar value.
     * @param isLeft If the scalar is on the left side of the operation.
     * @throws 'std::runtime_error': If the array does not store double values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarSub(double value, bool isLeft);

    /**
     * This method executes scalar multiplication. All elements of the array will be multiplied
     * with the provided value.
     * 
     * @param value The scalar value.
     * @throws 'std::runtime_error': If the array does not store 32-bit integer values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarMul(int32_t value);

    /**
     * This method executes scalar multiplication. All elements of the array will be multiplied
     * with the provided value.
     * 
     * @param value The scalar value.
     * @throws 'std::runtime_error': If the array does not store 64-bit integer values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarMul(int64_t value);

    /**
     * This method executes scalar multiplication. All elements of the array will be multiplied
     * with the provided value.
     * 
     * @param value The scalar value.
     * @throws 'std::runtime_error': If the array does not store float values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarMul(float value);

    /**
     * This method executes scalar multiplication. All elements of the array will be multiplied
     * with the provided value.
     * 
     * @param value The scalar value.
     * @throws 'std::runtime_error': If the array does not store double values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarMul(double value);

    /**
     * This method executes scalar division. All elements of the array will be divided
     * with the provided value.
     * 
     * @param value The scalar value.
     * @param isLeft If the scalar is on the left side of the operation.
     * @throws 'std::runtime_error': If the array does not store 32-bit integer values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarDiv(int32_t value, bool isLeft);

    /**
     * This method executes scalar subtraction. All elements of the array will be subtracted
     * with the provided value.
     * 
     * @param value The scalar value.
     * @param isLeft If the scalar is on the left side of the operation.
     * @throws 'std::runtime_error': If the array does not store 64-bit integer values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarDiv(int64_t value, bool isLeft);

    /**
     * This method executes scalar subtraction. All elements of the array will be subtracted
     * with the provided value.
     * 
     * @param value The scalar value.
     * @param isLeft If the scalar is on the left side of the operation.
     * @throws 'std::runtime_error': If the array does not store float values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarDiv(float value, bool isLeft);

    /**
     * This method executes scalar subtraction. All elements of the array will be subtracted
     * with the provided value.
     * 
     * @param value The scalar value.
     * @param isLeft If the scalar is on the left side of the operation.
     * @throws 'std::runtime_error': If the array does not store double values.
     * @return The result array as string in array processable format.
     */
    VarLen32 scalarDiv(double value, bool isLeft);

    static VarLen32 fill(int32_t value, Array &structure);

    static VarLen32 fill(int64_t value, Array &structure);

    static VarLen32 fill(float value, Array &structure);

    static VarLen32 fill(double value, Array &structure);

    static VarLen32 fill(std::string &value, Array &structure);

    std::string print();

};

#include "Array.tpp"

}
#endif