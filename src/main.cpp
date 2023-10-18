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
    auto tree = parser.parse();
    auto s = downcast<clpl::FuncDeclStmt>(tree[0]);
    auto vard = downcast<clpl::VarDeclStmt>(downcast<clpl::BlockStmt>(s->body)->statements[0]);
    std::cout << vard->type->toString() << "\n";
    return 0;
}