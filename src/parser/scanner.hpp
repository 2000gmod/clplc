#pragma once

#include "token.hpp"

#include <vector>
#include <unordered_map>

class Scanner {
    private:
        std::vector<Token> tokens;
        std::unordered_map<std::string, TokenT> keywords;

        int start = 0, current = 0, line = 1;
        std::string src;

    public:
        Scanner(const std::string &src);
        std::vector<Token> tokenize();

    private:
        void scanToken();
        bool match(char expected);
        char peek();
        char peekNext();
        bool isAlpha(char c);
        bool isDigit(char c);
        bool isAlphaNumeric(char c);
        char advance();
        bool atEnd();
        void addToken(const Token &tok);
        void addToken(const TokenT &tokt);
        void scanIdentifier();
        void scanNumber();
        void scanString();
        std::string formatEscapes(const std::string &val);
};