#include "../include/Array.h"

namespace lingodb::runtime {

    struct ArrayRuntime {

        static lingodb::runtime::VarLen32 fromString(lingodb::runtime::VarLen32 str, mlir::Type type);

        static VarLen32 append(VarLen32 left, VarLen32 right, mlir::Type leftType, mlir::Type rightType);
        static VarLen32 append(VarLen32 array, mlir::Type type, int32_t value);
        static VarLen32 append(VarLen32 array, mlir::Type type, int64_t value);
        static VarLen32 append(VarLen32 array, mlir::Type type, float value);
        static VarLen32 append(VarLen32 array, mlir::Type type, double value);
        static VarLen32 append(VarLen32 array, mlir::Type type, VarLen32 value);

        static VarLen32 slice(VarLen32 array, mlir::Type type, int32_t lowerBound, int32_t upperBound, int32_t dimension);

        static VarLen32 subscript(VarLen32 array, mlir::Type type, int32_t position);

        static VarLen32 add(VarLen32 left, VarLen32 right, mlir::Type leftType, mlir::Type rightType);
        static VarLen32 sub(VarLen32 left, VarLen32 right, mlir::Type leftType, mlir::Type rightType);
        static VarLen32 mul(VarLen32 left, VarLen32 right, mlir::Type leftType, mlir::Type rightType);
        static VarLen32 div(VarLen32 left, VarLen32 right, mlir::Type leftType, mlir::Type rightType);

        static VarLen32 scalarAdd(VarLen32 array, mlir::Type type, int32_t value);
        static VarLen32 scalarAdd(VarLen32 array, mlir::Type type, int64_t value);
        static VarLen32 scalarAdd(VarLen32 array, mlir::Type type, float value);
        static VarLen32 scalarAdd(VarLen32 array, mlir::Type type, double value);

        static VarLen32 scalarSub(VarLen32 array, mlir::Type type, int32_t value, bool isLeft);
        static VarLen32 scalarSub(VarLen32 array, mlir::Type type, int64_t value, bool isLeft);
        static VarLen32 scalarSub(VarLen32 array, mlir::Type type, float value, bool isLeft);
        static VarLen32 scalarSub(VarLen32 array, mlir::Type type, double value, bool isLeft);

        static VarLen32 scalarMul(VarLen32 array, mlir::Type type, int32_t value);
        static VarLen32 scalarMul(VarLen32 array, mlir::Type type, int64_t value);
        static VarLen32 scalarMul(VarLen32 array, mlir::Type type, float value);
        static VarLen32 scalarMul(VarLen32 array, mlir::Type type, double value);

        static VarLen32 scalarDiv(VarLen32 array, mlir::Type type, int32_t value, bool isLeft);
        static VarLen32 scalarDiv(VarLen32 array, mlir::Type type, int64_t value, bool isLeft);
        static VarLen32 scalarDiv(VarLen32 array, mlir::Type type, float value, bool isLeft);
        static VarLen32 scalarDiv(VarLen32 array, mlir::Type type, double value, bool isLeft);

    };

}