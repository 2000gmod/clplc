#include <iostream>
#include "parser.hpp"
#include "compiler.hpp"

int main() {
    auto test = R"(
        func main() -> void {
            var a : func(i32, i32 -> i32*)*;
            var b : i32 = 5;
            return 0;
        }
    )";
    clpl::Parser parser(test);
    return 0;
}