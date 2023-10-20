#pragma once

#include <utility>
#include <vector>

#include "token.hpp"
#include "statement.hpp"
#include <unordered_map>
#include "../util.hpp"

#define MAX_ARGS 16

namespace clpl {
struct ParseError : public std::exception {
        std::string msg;
        explicit ParseError(std::string msg) : msg(std::move(msg)) { }
    };

    using SList = std::vector<StmtSP>;

    class Parser {
        private:
            bool hadErrors = false;
            std::vector<Token> tokens;

            SList scopeStack;
            int scopeCount = 0;

            std::unordered_map<std::string, TypeSP> nTypes;
            std::vector<std::unordered_map<std::string, TypeSP>> identTypes;

            int current = 0;

        public:
            explicit Parser(const std::string &src);
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
            StmtSP blockStatement(const std::vector<clpl::ParameterT> &params = {});
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
            TypeSP parsePointerType(const TypeSP &type);
            
            // UTILITY

            bool isAtEnd();
            Token &advance();
            Token &consume(TokenT tokt, const std::string &msg);
            ParseError error(const Token &tok, const std::string &msg);
            bool check(TokenT tokt);
            bool match(TokenT tokt);
            bool match(const std::initializer_list<TokenT> &toks);
            Token &previous();
            Token &peek();
            bool checkForm(const std::initializer_list<TokenT> &toks);

            template<class T>
            bool isInsideScopeOf();

            bool exists(const std::string &name);
            TypeSP getTypeFromID(const std::string &name);
    };

    template <class T>
    bool Parser::isInsideScopeOf() {
        for (const auto &i : scopeStack) {
            if (instanceof<T>(i)) return true;
        }
        return false;
    }
}