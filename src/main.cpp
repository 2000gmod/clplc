#include <iostream>
#include "parser.hpp"
#include "compiler.hpp"

int main() {
    auto test = R"(
        func main() -> void {
            var a : func(i32, i32 -> i32*);
            return;
        }
    )";
    clpl::Parser parser(test);
    auto a = parser.parse();
    return 0;
}