#include <iostream>
#include "scanner.hpp"

int main() {
    Scanner scanner("var a : string = \"aaa\"");
    auto toks = scanner.tokenize();
    for (auto &i : toks) {
        std::cout << i.toString() << "\n";
    }
    return 0;
}