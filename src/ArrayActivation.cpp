#include "../include/Array.h"

using lingodb::runtime::Array;

lingodb::runtime::VarLen32 Array::sigmoid() {
    if (!isNumericType()) {
        throw std::runtime_error("Array-Sigmoid: Only supported with numeric types");
    }
    return executeActivationFunction<ArraySigmoidOperator>();
}