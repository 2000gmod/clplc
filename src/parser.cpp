#include "parser.hpp"

#include "scanner.hpp"

Parser::Parser(const std::string &src) {
    tokens = Scanner(src).tokenize();
}