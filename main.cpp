#include <iostream>
#include "VarLen32.h"
#include "Array.h"

int main() {
    std::string content1 = "{{},{{1,2,3},{1,2,3},{1,2,3},{},{1,2,3},{1,2,3}},{{1,2,3},{1,2,3},{1,2,3},{1,2,3},{1,2,3},{1,2,3},{}}}";
    std::string result1 = "";
    Array::fromString(content1, result1, mlir::Type::INTEGER);
    Array array1(result1, mlir::Type::INTEGER);
    std::cout << array1.print(mlir::Type::INTEGER) << std::endl;


    std::string content2 = "{{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}},{{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"},{\"Hello World\",\"A new world is born\",\"LingoDB is a Database\"}}}";
    std::string result2 = "";
    Array::fromString(content2, result2, mlir::Type::STRING);
    Array array2(result2, mlir::Type::STRING);
    std::cout << array2.print(mlir::Type::STRING) << std::endl;

    std::string content3 = "{{{{1,2,3},{4,5,6}},{{9},{10,11,12}}},{},{{{13,14,15},{16,18}},{{19,20,21},{22,23,24,37}}}}";
    std::string result3 = "";
    Array::fromString(content3, result3, mlir::Type::INTEGER);
    Array array3(result3, mlir::Type::INTEGER);
    std::cout << array3.print(mlir::Type::INTEGER) << std::endl;
    return 0;
}