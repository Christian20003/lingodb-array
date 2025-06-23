#include <iostream>
#include "VarLen32.h"
#include "Array.h"

int main() {
    std::string test = "Hello World";
    VarLen32 val(test);
    //Array("{1,2,3,4}", mlir::Type::INTEGER);
    std::string array = "{{{1,2,3},{1,2,3}},{{1,2,},{1,2,3}}}";
    std::string stringArray = "{{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"},{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}},{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"},{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}}}";
    std::string result = "";
    Array::fromString(stringArray, result, mlir::Type::STRING);
    return 0;
}