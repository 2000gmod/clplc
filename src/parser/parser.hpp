#pragma once

#include <vector>

#include "token.hpp"
#include "statement.hpp"
#include <unordered_map>
#include "../util.hpp"

#define MAX_ARGS 16

namespace clpl {
    struct ParseError {
        std::string msg;
        ParseError(const std::string &msg) : msg(msg) { }
    };

    using SList = std::vector<StmtSP>;

    class Parser {
        private:
            bool hadErrors = false;
            int numErrors = 0;
            std::vector<Token> tokens;
            SList scopeStack;
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
            StmtSP forStatement();
            StmtSP ifStatement();
            StmtSP returnStatement();
            StmtSP whileStatement();
            StmtSP blockStatement();
            StmtSP breakStatement();
            StmtSP continueStatement();
            StmtSP expressionStatement();

            ExprSP expression();

            ExprSP assignment();
            ExprSP orExpr();
            ExprSP andExpr();
            ExprSP eqExpr();
            ExprSP compExpr();
            ExprSP addition();
            ExprSP multiplication();
            ExprSP unary();
            ExprSP memberOpExpr();
            ExprSP callExpr(ExprSP callee);
            ExprSP primaryExpr();


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

            template<class T>
            bool isInsideScopeOf();
    };

    template <class T>
    bool Parser::isInsideScopeOf() {
        for (const auto &i : scopeStack) {
            if (instanceof<T>(i)) return true;
        }
        return false;
    }
}