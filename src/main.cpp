#include <iostream>
#include "parser.hpp"

int main() {
    auto test = "3*6 + 4*(a(2));";
    Parser parser(test);
    auto tree = parser.parse();
    return 0;
}