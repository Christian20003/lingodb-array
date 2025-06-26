#include <iostream>
#include "ArrayRuntime.h"

using lingodb::runtime::Array;
using lingodb::runtime::VarLen32;
using lingodb::runtime::ArrayRuntime;

int main() {
    std::string string1 = "{{},{{1,2,3},{1,2,3},{1,2,3},{},{1,2,3},{1,2,3}},{{1,2,3},{1,2,3},{1,2,3},{1,2,3},{1,2,3},{1,2,3},{}}}";
    VarLen32 var1 = VarLen32::fromString(string1);
    var1 = ArrayRuntime::fromString(var1, mlir::Type::INTEGER);
    
    std::string string2 = "{{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}},{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"},{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}}}";
    VarLen32 var2 = VarLen32::fromString(string2);
    var2 = ArrayRuntime::fromString(var2, mlir::Type::STRING);
    
    std::string string3 = "{{{{1,2,3},{4,5,6}},{{9},{10,11,12}}},{{},null},{{{13,14,15},{16,18}},{{19,20,21},{22,23,24,37}}}}";
    VarLen32 var3 = VarLen32::fromString(string3);
    var3 = ArrayRuntime::fromString(var3, mlir::Type::INTEGER);
    
    std::string array = var1.str();
    Array result(array, mlir::Type::INTEGER);
    std::cout << result.print(mlir::Type::INTEGER) << std::endl;
    return 0;
}