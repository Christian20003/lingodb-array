#include <cstdint>
#include <string>

namespace lingodb::runtime {

struct ArrayAddOperator {

    /**
     * This function executes an addition operation on a list of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param left A pointer to a value of type `TYPE`.
     * @param right A pointer to a value of type `TYPE`.
     * @param size The length of both value lists.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param scalarLeft If the left parameter points to a single element.
     * @param scalarRight If the right parameter points to a single element.
     */
    template<class TYPE>
    static void Operator(const TYPE *left, const TYPE *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight) {
        for (size_t i = 0; i < size; i++) {
            TYPE result = *left + *right;
            memcpy(buffer, &result, sizeof(TYPE));
            buffer += sizeof(TYPE);
            if (!scalarLeft) left++;
            if (!scalarRight) right++;
        }
    }
};

struct ArraySubOperator{

    /**
     * This function executes a subtraction operation on a list of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param left A pointer to a value of type `TYPE`.
     * @param right A pointer to a value of type `TYPE`.
     * @param size The length of both value lists.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param scalarLeft If the left parameter points to a single element.
     * @param scalarRight If the right parameter points to a single element.
     */
    template<class TYPE>
    static void Operator(const TYPE *left, const TYPE *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight) {
        for (size_t i = 0; i < size; i++) {
            TYPE result = *left - *right;
            memcpy(buffer, &result, sizeof(TYPE));
            buffer += sizeof(TYPE);
            if (!scalarLeft) left++;
            if (!scalarRight) right++;
        }
    }
};

struct ArrayMulOperator{

    /**
     * This function executes an multiplication operation on a list of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param left A pointer to a value of type `TYPE`.
     * @param right A pointer to a value of type `TYPE`.
     * @param size The length of both value lists.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param scalarLeft If the left parameter points to a single element.
     * @param scalarRight If the right parameter points to a single element.
     */
    template<class TYPE>
    static void Operator(const TYPE *left, const TYPE *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight) {
        for (size_t i = 0; i < size; i++) {
            TYPE result = *left * *right;
            memcpy(buffer, &result, sizeof(TYPE));
            buffer += sizeof(TYPE);
            if (!scalarLeft) left++;
            if (!scalarRight) right++;
        }
    }
};

struct ArrayDivOperator{

    /**
     * This function executes an division operation on a list of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param left A pointer to a value of type `TYPE`.
     * @param right A pointer to a value of type `TYPE`.
     * @param size The length of both value lists.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param scalarLeft If the left parameter points to a single element.
     * @param scalarRight If the right parameter points to a single element.
     */
    template<class TYPE>
    static void Operator(const TYPE *left, const TYPE *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight) {
        for (size_t i = 0; i < size; i++) {
            TYPE result = *left / *right;
            memcpy(buffer, &result, sizeof(TYPE));
            buffer += sizeof(TYPE);
            if (!scalarLeft) left++;
            if (!scalarRight) right++;
        }
    }

};

struct ArraySigmoidOperator {

    /**
     * This function executes the sigmoid function on a list of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param data A pointer to the first entry of a list of values.
     * @param size The length of the given list.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     */
    template <class TYPE>
	static void Operator(const TYPE *data, uint32_t size, char *&buffer) {
		for(size_t i = 0; i < size; i++) {
			TYPE result = 1 / (1 + std::exp(-(*data++)));
            memcpy(buffer, &result, sizeof(TYPE));
            buffer += sizeof(TYPE);
		}
	}
};

}