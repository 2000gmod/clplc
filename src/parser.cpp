#include "parser.hpp"

#include "scanner.hpp"
#include "util.hpp"

#include <iostream>

Parser::Parser(const std::string &src) {
    tokens = Scanner(src).tokenize();

    nTypes = {
        {"void", std::make_shared<NamedType>("void")},
        {"i32", std::make_shared<NamedType>("i32")},
        {"double", std::make_shared<NamedType>("double")},
    };
}

SList Parser::parse() {
    SList statments;
    try {
        while (!isAtEnd()) {
            statments.push_back(topLevelStatement());
        }
    }
    catch (ParseError &e) { 
        std::cerr << e.msg << "\n";
    }
}

StmtSP Parser::topLevelStatement() {
    return declaration();
}

StmtSP Parser::declaration() {
    if (match({TokenT::FUNC, TokenT::METHOD, TokenT::OPERATOR})) return functionDecl();
    if (match(TokenT::VAR)) return variableDecl();
    return statement();
}

StmtSP Parser::functionDecl() {
    if (!scopeStack.empty()) throw error(peek(), "Function declarations must be at global scope.");

    auto name = consume(TokenT::IDENTIFIER, "Expected function identifier.");
    consume(TokenT::LEFT_PAREN, "Expected '(' after function identifier.");

    std::vector<ParameterT> params;
    if (!check(TokenT::RIGHT_PAREN)) do {
        if (params.size() >= MAX_ARGS) {
            throw error(peek(), "Exceeded max parameter count.");
        }
        auto pname = consume(TokenT::IDENTIFIER, "Expected parameter identifier.");
        consume(TokenT::COLON, "Expected ':' after parameter identifier.");
        auto ptype = parseType();
        ParameterT param = {ptype, pname};
        params.push_back(param);
    } while (match(TokenT::COMMA));

    consume(TokenT::RIGHT_PAREN, "Expected ')' after function parameter list.");
    auto rtype = nTypes["void"];
    if (match(TokenT::COLON)) {
        rtype = parseType();
    }

    StmtSP fbody = nullptr;
    if (match(TokenT::LEFT_CUR)) {
        scopeStack.push(std::make_shared<FuncDeclStmt>(rtype, name, params, nullptr));
        fbody = blockStatement();
        scopeStack.pop();
    }
    else consume(TokenT::SEMICOLON, "Expected ';' after external (bodyless) function declaration.");
    return std::make_shared<FuncDeclStmt>(rtype, name, params, fbody);
}

StmtSP Parser::variableDecl() {
    auto name = consume(TokenT::IDENTIFIER, "Expected variable identifier.");
    consume(TokenT::COLON, "Expected ':' after variable identifier.");
    auto vartype = parseType();

    ExprSP value = nullptr;
    if (match(TokenT::ASSIGN)) value = expression();
    consume(TokenT::SEMICOLON, "Expected ';' after variable declaration.");
    return std::make_shared<VarDeclStmt>(vartype, name, value);
}

StmtSP Parser::statement() {
    
}