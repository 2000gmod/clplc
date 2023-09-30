#pragma once

#include <vector>
#include <stack>

#include "token.hpp"
#include "statement.hpp"
#include <unordered_map>

struct ParseError {
    std::string msg;
    ParseError(const std::string &msg) : msg(msg) { }
};

using SList = std::vector<StmtSP>;

#define MAX_ARGS 16

class Parser {
    private:
        std::vector<Token> tokens;
        std::stack<StmtSP> scopeStack;
        std::unordered_map<std::string, TypeSP> nTypes;

        int current = 0;

    public:
        Parser(const std::string &src);
        SList parse();

    private:
        StmtSP topLevelStatement();
        StmtSP declaration();
        StmtSP functionDecl();
        StmtSP variableDecl();

        StmtSP statement();


        TypeSP parseType();
        TypeSP parseNamedType();
        

        // UTILITY

        bool isAtEnd();
        Token &advance();
        Token &consume(const TokenT tokt, const std::string &msg);
        ParseError error(const Token &tok, const std::string &msg);
        bool check(const TokenT tokt);
        bool match(const TokenT tokt);
        bool match(const std::initializer_list<TokenT> &toks);
        Token &previous();
        Token &peek();
        bool checkForm(const std::initializer_list<TokenT> &toks);
};