#pragma once

#include <vector>

#include "token.hpp"

class Parser {
    private:
        std::vector<Token> tokens;

    public:
        Parser(const std::string &src);

    private:

};