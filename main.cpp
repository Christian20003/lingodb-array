#include <iostream>
#include "ArrayRuntime.h"

using lingodb::runtime::Array;
using lingodb::runtime::VarLen32;
using lingodb::runtime::ArrayRuntime;

int main() {
    // std::string string1 = "{{{{1,2,3},{4,5,6}},{{9},{10,11,12}}},{{},null},{{{13,14,15},{16,18}},{{19,20,21},{22,23,24,37}}}}";
    // std::string string1 = "{{{1,1},{2,2}},{{3,3},{4,4}},{{5,5},{6,6}}}";
    std::string string1 = "{{{{1,2,3},{4,5,6},{7,8,9},{10,11,12}},{{13,14,15},{16,17,18},{19,20,21},{22,23,24}}},{{{25,26,27},{28,29,30},{31,32,33},{34,35,36}},{{37,38,39},{40,41,42},{43,44,45},{46,47,48}}},{{{49,50,51},{52,53,54},{55,56,57},{58,59,60}},{{61,62,63},{64,65,66},{67,68,69},{70,71,72}}}}";
    VarLen32 var1 = VarLen32::fromString(string1);
    var1 = ArrayRuntime::fromString(var1, mlir::Type::INTEGER);
    
    std::string string2 = "{{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}},{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"},{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}}}";
    VarLen32 var2 = VarLen32::fromString(string2);
    var2 = ArrayRuntime::fromString(var2, mlir::Type::STRING);
    
    std::string string3 = "{2,3}";
    VarLen32 var3 = VarLen32::fromString(string3);
    var3 = ArrayRuntime::fromString(var3, mlir::Type::INTEGER);

    std::string string4 = "Hello World";
    VarLen32 var4 = VarLen32::fromString(string4);
    // var4 = ArrayRuntime::fromString(var4, mlir::Type::STRING);
    
    // VarLen32 op = ArrayRuntime::append(var1, var3, mlir::Type::INTEGER, mlir::Type::INTEGER);
    // VarLen32 op = ArrayRuntime::append(var1, mlir::Type::INTEGER, 4);
    // VarLen32 op2 = ArrayRuntime::slice(var1, mlir::Type::INTEGER, 1, 2, 4);
    // VarLen32 op = ArrayRuntime::subscript(var2, mlir::Type::INTEGER, 1);
    // VarLen32 op = ArrayRuntime::div(var3, var3, mlir::Type::INTEGER, mlir::Type::INTEGER);
    // VarLen32 op = ArrayRuntime::scalarSub(var3, mlir::Type::INTEGER, 5, true);
    // VarLen32 op = ArrayRuntime::fill(var3, mlir::Type::INTEGER);
    VarLen32 op = ArrayRuntime::sigmoid(var1, mlir::Type::INTEGER);
    std::string array = op.str();
    //std::cout << array << std::endl;
    Array result(array, mlir::Type::INTEGER);
    std::cout << result.print() << std::endl;
    return 0;
}