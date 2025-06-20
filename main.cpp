#include <iostream>
#include "VarLen32.h"
#include "Array.h"

int main() {
    std::string test = "Hello World";
    VarLen32 val(test);
    Array("{1,2,3,4}", mlir::Type::INTEGER);
    std::cout << val.str() << std::endl;
    return 0;
}