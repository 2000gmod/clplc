#include <iostream>
#include "parser.hpp"
#include "compiler.hpp"

int main() {
    auto test = R"(
        func main() -> void {
            var a : i32 = 0;
            var b : i32 = 0;

            while (a < 10) {
                while (b < 10) {
                    b = b + 1;
                }
                a = a + 1;
            }
        }
    )";
    clpl::Parser parser(test);
    auto a = parser.parse();
    clpl::Compiler("test", a).compile();
    return 0;
}