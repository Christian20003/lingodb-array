#include <iostream>
#include "VarLen32.h"
#include "Array.h"

int main() {
    std::string content1 = "{{null,{null,5,null}},{{1,2,3},{1,2,3}}}";
    std::string result1 = "";
    Array::fromString(content1, result1, mlir::Type::INTEGER);
    Array array1(result1, mlir::Type::INTEGER);
    std::cout << array1.print(mlir::Type::INTEGER) << std::endl;


    std::string content2 = "{{{\"Hello World\",\"A new world is born\",null},{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}},{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"},{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}}}";
    std::string result2 = "";
    Array::fromString(content2, result2, mlir::Type::STRING);
    Array array2(result2, mlir::Type::STRING);
    std::cout << array2.print(mlir::Type::STRING) << std::endl;
    return 0;
}