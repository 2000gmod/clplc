#include "parser.hpp"

#include "scanner.hpp"

#include <iostream>

using namespace clpl;

Parser::Parser(const std::string &src) {
    tokens = Scanner(src).tokenize();

    nTypes = {
        {"void", std::make_shared<NamedType>("void")},
        {"bool", std::make_shared<NamedType>("bool")},
        {"i8", std::make_shared<NamedType>("i8")},
        {"i16", std::make_shared<NamedType>("i16")},
        {"i32", std::make_shared<NamedType>("i32")},
        {"i64", std::make_shared<NamedType>("i64")},
        {"u8", std::make_shared<NamedType>("u8")},
        {"u16", std::make_shared<NamedType>("u16")},
        {"u32", std::make_shared<NamedType>("u32")},
        {"u64", std::make_shared<NamedType>("u64")},
        {"f32", std::make_shared<NamedType>("f32")},
        {"f64", std::make_shared<NamedType>("f64")},
        {"ptr", std::make_shared<NamedType>("ptr")},
    };

    identTypes.emplace_back();
}

SList Parser::parse() {
    SList statements;
    while (!isAtEnd()) {
        try {
            statements.push_back(topLevelStatement());
        }
        catch (ParseError &e) {
            hadErrors = true;
            std::cerr << e.msg << "\n";
            break;
        }
    }
    if (hadErrors) {
        std::cerr << "\033[1;31mHad unrecoverable errors while parsing this file.\033[0m\n";
        std::exit(EXIT_FAILURE);
    }
    return statements;
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
    std::vector<TypeSP> paramTypes;
    if (!check(TokenT::RIGHT_PAREN)) do {
        if (params.size() >= MAX_ARGS) {
            throw error(peek(), "Exceeded max parameter count.");
        }
        auto pname = consume(TokenT::IDENTIFIER, "Expected parameter identifier.");
        consume(TokenT::COLON, "Expected ':' after parameter identifier.");
        auto ptype = parseType();

        ParameterT param = {ptype, pname};
        params.push_back(param);
        paramTypes.push_back(ptype);
    } while (match(TokenT::COMMA));

    consume(TokenT::RIGHT_PAREN, "Expected ')' after function parameter list.");
    auto rtype = nTypes["void"];
    if (match(TokenT::ARROW)) {
        rtype = parseType();
    }

    if (!exists(name.identName)) {
        auto ftype = std::make_shared<FunctionReferenceType>(rtype, paramTypes);
        identTypes[scopeCount].insert({name.identName, ftype});
    }
    else throw error(name, "Function redefinition.");

    StmtSP fbody = nullptr;
    if (match(TokenT::LEFT_CUR)) {
        scopeStack.push_back(std::make_shared<FuncDeclStmt>(rtype, name, params, nullptr));
        fbody = blockStatement(params);
        scopeStack.pop_back();
    }
    else consume(TokenT::SEMICOLON, "Expected ';' after external (bodyless) function declaration.");
    return std::make_shared<FuncDeclStmt>(rtype, name, params, fbody);
}

StmtSP Parser::variableDecl() {
    auto name = consume(TokenT::IDENTIFIER, "Expected variable identifier.");
    consume(TokenT::COLON, "Expected type declaration.");
    auto vartype = parseType();

    ExprSP value = nullptr;
    if (match(TokenT::ASSIGN)) value = expression();
    consume(TokenT::SEMICOLON, "Expected ';' after variable declaration.");
    if (!exists(name.identName)) {
        identTypes[scopeCount].insert({name.identName, vartype});
    }
    else {
        throw error(name, "Variable already defined.");
    }
    return std::make_shared<VarDeclStmt>(vartype, name, value);
}

StmtSP Parser::statement() {
    if (!isInsideScopeOf<FuncDeclStmt>()) throw error(peek(), "Illegal global scope statement.");
    if (match(TokenT::FOR)) return forStatement();
    if (match(TokenT::IF)) return ifStatement();
    if (match(TokenT::RETURN)) return returnStatement();
    if (match(TokenT::WHILE)) return whileStatement();
    if (match(TokenT::LEFT_CUR)) return blockStatement();
    if (match(TokenT::BREAK)) return breakStatement();
    if (match(TokenT::CONTINUE)) return continueStatement();

    return expressionStatement();
}

StmtSP Parser::forStatement() {
    consume(TokenT::LEFT_PAREN, "Expected '(' after 'for'.");
    scopeStack.push_back(std::make_shared<ForStmt>());
    StmtSP init;
    if (checkForm({TokenT::IDENTIFIER, TokenT::COLON})) {
        auto name = consume(TokenT::IDENTIFIER, "Expected identifier.");
        consume(TokenT::COLON, "Expected ':'.");
        auto type = parseType();
        consume(TokenT::ASSIGN, "Expected assignment in for-loop initializer.");
        init = std::make_shared<VarDeclStmt>(type, name, expression());
        consume(TokenT::SEMICOLON, "Expected ';' after for-loop initializer statement.");
    }
    else if (match(TokenT::SEMICOLON)) {
        init = nullptr;
    }
    else init = expressionStatement();

    ExprSP condition = nullptr;
    if (!check(TokenT::SEMICOLON)) condition = expression();
    
    consume(TokenT::SEMICOLON, "Expected ';' after for-loop condition");

    ExprSP increment = nullptr;
    if (!check(TokenT::RIGHT_PAREN)) increment = expression();
    
    consume(TokenT::RIGHT_PAREN, "Expected ')' after for-loop increment.");

    StmtSP body = statement();
    scopeStack.pop_back();
    return std::make_shared<ForStmt>(init, condition, increment, body);
}

StmtSP Parser::ifStatement() {
    consume(TokenT::LEFT_PAREN, "Expected '(' after 'if'.");
    scopeStack.push_back(std::make_shared<IfStmt>());

    ExprSP condition = expression();
    consume(TokenT::RIGHT_PAREN, "Expected ')' after condition.");
    StmtSP ifBody = statement();
    StmtSP elseBody = nullptr;
    if (match(TokenT::ELSE)) {
        elseBody = statement();
    }
    scopeStack.pop_back();
    return std::make_shared<IfStmt>(condition, ifBody, elseBody);
}

StmtSP Parser::returnStatement() {
    ExprSP value = nullptr;
    if (!check(TokenT::SEMICOLON)) {
        value = expression();
    }

    consume(TokenT::SEMICOLON, "Expected ';' after return value.");
    return std::make_shared<ReturnStmt>(value);
}

StmtSP Parser::whileStatement() {
    consume(TokenT::LEFT_PAREN, "Expected '(' after 'while'.");
    scopeStack.push_back(std::make_shared<WhileStmt>());
    ExprSP condition = expression();
    consume(TokenT::RIGHT_PAREN, "Expected ')' after condition.");
    StmtSP body = statement();
    scopeStack.pop_back();
    return std::make_shared<WhileStmt>(condition, body);
}

StmtSP Parser::blockStatement(const std::vector<ParameterT> &params) {
    scopeStack.push_back(std::make_shared<BlockStmt>());
    identTypes.emplace_back();
    scopeCount++;

    if (!params.empty()) for (auto &i: params) {
        if (!exists(i.name.identName)) {
            identTypes[scopeCount].insert({i.name.identName, i.type});
        } else {
            throw error(i.name, "Name already defined.");
        }
    }


    SList body;
    while (!check(TokenT::RIGHT_CUR) && !isAtEnd()) {
        body.push_back(topLevelStatement());
    }
    consume(TokenT::RIGHT_CUR, "Expected '}' after a block statement.");
    scopeStack.pop_back();
    identTypes.pop_back();
    scopeCount--;
    return std::make_shared<BlockStmt>(body);
}

StmtSP Parser::breakStatement() {
    if (isInsideScopeOf<WhileStmt>() || isInsideScopeOf<ForStmt>()) {
        consume(TokenT::SEMICOLON, "Expected ';'.");
        return std::make_shared<BreakStmt>();
    }
    else throw error(previous(), "Break statement needs to be inside a loop.");
}

StmtSP Parser::continueStatement() {
    if (isInsideScopeOf<WhileStmt>() || isInsideScopeOf<ForStmt>()) {
        consume(TokenT::SEMICOLON, "Expected ';'.");
        return std::make_shared<ContinueStmt>();
    }
    else throw error(previous(), "Continue statement needs to be inside a loop.");
}

StmtSP Parser::expressionStatement() {
    auto expr = std::make_shared<ExprStmt>(expression());
    consume(TokenT::SEMICOLON, "Expected ';'.");
    return expr;
}

ExprSP Parser::expression() {
    return assignment();
}

ExprSP Parser::assignment() {
    auto expr = orExpr();

    if (match(TokenT::ASSIGN)) {
        auto equals = previous();
        auto value = assignment();

        if (instanceof<IdentifierExpr>(expr)) {
            return std::make_shared<AssignExpr>(expr, value);
        }
        throw error(equals, "Invalid assignment target.");
    }
    return expr;
}

ExprSP Parser::orExpr() {
    auto expr = andExpr();

    while (match(TokenT::OR)) {
        auto op = previous().type;
        auto rhs = andExpr();
        expr = std::make_shared<BinaryExpr>(expr, rhs, op);
    }
    return expr;
}

ExprSP Parser::andExpr() {
    auto expr = eqExpr();

    while (match(TokenT::AND)) {
        auto op = previous().type;
        auto rhs = eqExpr();
        expr = std::make_shared<BinaryExpr>(expr, rhs, op);
    }
    return expr;
}

ExprSP Parser::eqExpr() {
    auto expr = compExpr();

    while (match({TokenT::EQ, TokenT::NOT_EQ})) {
        auto op = previous().type;
        auto rhs = compExpr();
        expr = std::make_shared<BinaryExpr>(expr, rhs, op);
    }
    return expr;
}

ExprSP Parser::compExpr() {
    auto expr = addition();

    while (match({TokenT::GT, TokenT::LT, TokenT::GEQ, TokenT::LEQ})) {
        auto op = previous().type;
        auto rhs = addition();
        expr = std::make_shared<BinaryExpr>(expr, rhs, op);
    }
    return expr;
}

ExprSP Parser::addition() {
    auto expr = multiplication();

    while (match({TokenT::PLUS, TokenT::MINUS})) {
        auto op = previous().type;
        auto rhs = multiplication();
        expr = std::make_shared<BinaryExpr>(expr, rhs, op);
    }
    return expr;
}

ExprSP Parser::multiplication() {
    auto expr = unary();

    while (match({TokenT::STAR, TokenT::SLASH, TokenT::MOD})) {
        auto op = previous().type;
        auto rhs = unary();
        expr = std::make_shared<BinaryExpr>(expr, rhs, op);
    }
    return expr;
}

ExprSP Parser::unary() {
    if (match({TokenT::NOT, TokenT::MINUS})) {
        auto op = previous().type;
        auto rhs = unary();
        return std::make_shared<UnaryExpr>(rhs, op);
    }
    return memberOpExpr();
}

ExprSP Parser::memberOpExpr() {
    auto expr = primaryExpr();

    while (true) {
        if (match(TokenT::LEFT_PAREN)) expr = callExpr(expr);
        else break;
    }
    return expr;
}

ExprSP Parser::callExpr(ExprSP callee) {
    std::vector<ExprSP> args;
    if (!check(TokenT::RIGHT_PAREN)) {
        do {
            if (args.size() > MAX_ARGS) {
                throw error(peek(), "Exceeded max argument count.");
            }
            args.push_back(expression());
        } while (match(TokenT::COMMA));
    }
    consume(TokenT::RIGHT_PAREN, "Expected ')'.");
    return std::make_shared<CallExpr>(callee, args);
}

ExprSP Parser::primaryExpr() {
    if (match({TokenT::BOOL_LIT, TokenT::INT_LIT, TokenT::DOUBLE_LIT, TokenT::STRING_LIT})) {
        auto expr = std::make_shared<LiteralExpr>(previous());
        TypeSP etype;
        switch(previous().type) {
            case TokenT::BOOL_LIT:
                etype = nTypes["bool"];
                break;
            case TokenT::INT_LIT:
                etype = nTypes["i32"];
                break;
            case TokenT::DOUBLE_LIT:
                etype = nTypes["f64"];
                break;
            case TokenT::STRING_LIT:
                etype = std::make_shared<IndexedPointerType>(nTypes["u8"]);
                break;
            default:
                break;
        }
        expr->type = etype;
        return expr;
    }

    if (match(TokenT::IDENTIFIER)) {
        auto expr = std::make_shared<IdentifierExpr>(previous());
        expr->type = getTypeFromID(previous().identName);
        return expr;
    }

    if (match(TokenT::LEFT_PAREN)) {
        auto expr = expression();
        consume(TokenT::RIGHT_PAREN, "Expected ')'.");
        auto out = std::make_shared<GroupExpr>(expr);
        out->type = expr->type;
        return out;
    }
    throw error(peek(), "Expected expression.");
}

/*
    ################################################################
                    TYPE PARSING
    ################################################################
*/

TypeSP Parser::parseType() {
    if (check(TokenT::IDENTIFIER)) {
        auto type = parseNamedType();
        return parsePointerType(type);
    }
    else if (match(TokenT::FUNC)) {
        consume(TokenT::LEFT_PAREN, "Expected '('.");
        std::vector<TypeSP> argTypes;
        if (!check(TokenT::RIGHT_PAREN)) {
            do {
                argTypes.push_back(parseType());
            } while (match(TokenT::COMMA));
        }
        consume(TokenT::ARROW, "Expected '->'.");
        auto rtype = parseType();
        consume(TokenT::RIGHT_PAREN, "Expected ')' after argument type list.");
        TypeSP ref = std::make_shared<FunctionReferenceType>(rtype, argTypes);
        return parsePointerType(ref);
    }
    else if (match(TokenT::LEFT_PAREN)) {
        auto type = parseType();
        consume(TokenT::RIGHT_PAREN, "Expected closing ')' after type.");
        return parsePointerType(type);
    }
    else throw error(peek(), "Unexpected type sequence.");
}


TypeSP Parser::parseNamedType() {
    auto name = consume(TokenT::IDENTIFIER, "Expected type identifier.");
    if (!nTypes.contains(name.identName)) {
        throw error(previous(), "Unknown type: '" + name.identName + "'.");
    }
    return nTypes[name.identName];
}

TypeSP Parser::parsePointerType(const TypeSP &type) {
    TypeSP out = type;
    do {
        while (match(TokenT::LEFT_SQR)) {
            consume(TokenT::RIGHT_SQR, "Expected ']'.");
            out = std::make_shared<IndexedPointerType>(out);
        }
        while (match(TokenT::STAR)) {
            out = std::make_shared<ReferencePointerType>(out);
        }

    } while (check(TokenT::LEFT_SQR) || check(TokenT::STAR));
    return out;
}