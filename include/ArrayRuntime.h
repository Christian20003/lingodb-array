#include "../include/Array.h"

namespace lingodb::runtime {

    struct ArrayRuntime {

        static lingodb::runtime::VarLen32 fromString(lingodb::runtime::VarLen32 str, mlir::Type type);

        static VarLen32 append(VarLen32 left, VarLen32 right, mlir::Type leftType, mlir::Type rightType);

    };

}