#include <iostream>
#include "ArrayRuntime.h"

using lingodb::runtime::Array;
using lingodb::runtime::VarLen32;
using lingodb::runtime::ArrayRuntime;

int main() {
    std::string string1 = "[2:4][2:3][2:3][-5:0]={{{{1,2,3},{4,5,6}},{{9},{10,11,12}}},{{},{}},{{{13,14,15},{16,18}},{{19,20,21},{22,23,24,37}}}}";
    // std::string string1 = "{{{1,1},{2,2}},{{3,3},{4,4}},{{5,5},{6,6}}}";
    //std::string string1 = "[-2:0][1:300] = {{70,6,5},{80,2,null},{4,5,6}}";
    VarLen32 var1 = VarLen32::fromString(string1);
    var1 = ArrayRuntime::fromString(var1, 0);
    
    std::string string2 = "{{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}},{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"},{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}}}";
    VarLen32 var2 = VarLen32::fromString(string2);
    var2 = ArrayRuntime::fromString(var2, 5);
    
    std::string string3 = "{{1,2},{3,4,5}}";
    VarLen32 var3 = VarLen32::fromString(string3);
    var3 = ArrayRuntime::fromString(var3, 0);

    std::string string4 = "Hello World";
    VarLen32 var4 = VarLen32::fromString(string4);
    // var4 = ArrayRuntime::fromString(var4, mlir::Type::STRING);

    std::string string5 = "{{1,2,3}}";
    VarLen32 var5 = VarLen32::fromString(string5);
    var5 = ArrayRuntime::fromString(var5, 0);

    std::string string6 = "[6:700][4:9]={{2.5,8.2},{5.2,8.2}}";
    VarLen32 var6 = VarLen32::fromString(string6);
    var6 = ArrayRuntime::fromString(var6, 3);
    
    // VarLen32 op = ArrayRuntime::append(var1, 0);
    VarLen32 op = ArrayRuntime::append(var2, 5, var4, true);
    //VarLen32 op = ArrayRuntime::slice(var1, 0, 2, 3, 1);
    //VarLen32 op = ArrayRuntime::subscript(var2, 5, 2);
    //op = ArrayRuntime::subscript(op, 5, 2);
    //op = ArrayRuntime::subscript(op, 0, 2);
    //op = ArrayRuntime::subscript(op, 0, 6);
    //std::cout << op.str() << std::endl;
    // VarLen32 op = ArrayRuntime::mul(var3, var3, 0, 0);
    // VarLen32 op = ArrayRuntime::scalarSub(var3, 0, 5, true);
    // VarLen32 op = ArrayRuntime::fill(var3, 0);
    // VarLen32 op = ArrayRuntime::sigmoid(var5, 3);
    // VarLen32 op = ArrayRuntime::matrixMul(var6, var5, 3, 3);
    // VarLen32 op = ArrayRuntime::cast(var2, 5, 0);
    // op = ArrayRuntime::cast(op, 5, 3);
    // VarLen32 op = ArrayRuntime::transpose(var5, 0);
    // VarLen32 op = ArrayRuntime::increment(var2, 5);
    std::string array = op.str();
    int pos = ArrayRuntime::getHighestPosition(var1, 0);
    //std::cout << array << std::endl;
    Array result(array);
    std::cout << result.print() << std::endl;
    std::cout << pos << std::endl;
    return 0;
}