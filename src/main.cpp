#include <iostream>
#include "parser.hpp"
#include "compiler.hpp"

int main() {
    auto test = R"(
        func main() {
            var a : i32[]*;
            return 0;
        }
    )";
    clpl::Parser parser(test);
    auto tree = parser.parse();
    return 0;
}