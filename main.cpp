#include <iostream>
#include "ArrayRuntime.h"

using lingodb::runtime::Array;
using lingodb::runtime::VarLen32;
using lingodb::runtime::ArrayRuntime;

int main() {
    // std::string string1 = "{{{{1,2,3},{4,5,6}},{{9},{10,11,12}}},{{},null},{{{13,14,15},{16,18}},{{19,20,21},{22,23,24,37}}}}";
    // std::string string1 = "{{{1,1},{2,2}},{{3,3},{4,4}},{{5,5},{6,6}}}";
    std::string string1 = "[-2:-1][1:300] = {{80,2,null},{4,5,6}}";
    VarLen32 var1 = VarLen32::fromString(string1);
    var1 = ArrayRuntime::fromString(var1, mlir::Type::INTEGER);
    
    /* std::string string2 = "{{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}},{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"},{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}}}";
    VarLen32 var2 = VarLen32::fromString(string2);
    var2 = ArrayRuntime::fromString(var2, mlir::Type::STRING);
    
    std::string string3 = "{2,3}";
    VarLen32 var3 = VarLen32::fromString(string3);
    var3 = ArrayRuntime::fromString(var3, mlir::Type::INTEGER);

    std::string string4 = "Hello World";
    VarLen32 var4 = VarLen32::fromString(string4);
    // var4 = ArrayRuntime::fromString(var4, mlir::Type::STRING);

    std::string string5 = "{2.5,6.5,7.2}";
    VarLen32 var5 = VarLen32::fromString(string5);
    var5 = ArrayRuntime::fromString(var5, mlir::Type::FLOAT);

    std::string string6 = "{{2.5,8.2,5.2}}";
    VarLen32 var6 = VarLen32::fromString(string6);
    var6 = ArrayRuntime::fromString(var6, mlir::Type::FLOAT); */
    
    // VarLen32 op = ArrayRuntime::append(var1, var3, mlir::Type::INTEGER, mlir::Type::INTEGER);
    // VarLen32 op = ArrayRuntime::append(var1, mlir::Type::INTEGER, 4);
    // VarLen32 op2 = ArrayRuntime::slice(var1, mlir::Type::INTEGER, 1, 2, 4);
    // VarLen32 op = ArrayRuntime::subscript(var2, mlir::Type::INTEGER, 1);
    // VarLen32 op = ArrayRuntime::div(var3, var3, mlir::Type::INTEGER, mlir::Type::INTEGER);
    // VarLen32 op = ArrayRuntime::scalarSub(var3, mlir::Type::INTEGER, 5, true);
    // VarLen32 op = ArrayRuntime::fill(var3, mlir::Type::INTEGER);
    // VarLen32 op = ArrayRuntime::sigmoid(var1, mlir::Type::INTEGER);
    // VarLen32 op = ArrayRuntime::matrixMul(var6, var5, mlir::Type::FLOAT, mlir::Type::FLOAT);
    std::string array = var1.str();
    // int pos = ArrayRuntime::getHighestPosition(var2, mlir::Type::STRING);
    //std::cout << array << std::endl;
    Array result(array, mlir::Type::INTEGER);
    // std::cout << result.print() << std::endl;
    // std::cout << pos << std::endl;
    return 0;
}