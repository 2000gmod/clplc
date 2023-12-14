#include "parser.hpp"

using namespace clpl;

bool Parser::isAtEnd() {
    return peek().type == TokenT::EOFILE;
}

Token &Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token &Parser::consume(const TokenT tokt, const std::string &msg) {
    if (check(tokt)) return advance();
    throw error(peek(), msg);
}

ParseError Parser::error(const Token &tok, const std::string &msg) {
    auto message = "(at token " + tok.toString() + ")\033[0m\n";
    message = "\033[1;31mError: (at line " + std::to_string(tok.line) + ") " + message;
    message += "\t\033[1m" + msg + "\033[0m"; 
    return ParseError(message);
}

bool Parser::check(const TokenT tokt) {
    if (isAtEnd()) return false;
    return tokt == peek().type;
}

bool Parser::match(const TokenT tokt) {
    if (check(tokt)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const std::initializer_list<TokenT> &toklist) {
    for (const auto &i : toklist) {
        if (check(i)) {
            advance();
            return true;
        }
    }
    return false;
}

Token &Parser::previous() {
    return tokens[current - 1];
}

Token &Parser::peek() {
    return tokens[current];
}

bool Parser::checkForm(const std::initializer_list<TokenT> &toklist) {
    int prevCurrent = current;

    for (const auto &i : toklist) {
        if (!match(i)) {
            current = prevCurrent;
            return false;
        }
    }

    current = prevCurrent;
    return true;
}

bool Parser::exists(const std::string &name) {
    for (auto &map : identTypes) {
        if (map.contains(name)) return true;
    }
    return false;
}

TypeSP Parser::getTypeFromID(const std::string &name) {
    for (auto &map : identTypes) {
        if (map.contains(name)) return map[name];
    }
    
    throw error(previous(), "Unknown identifier.");
}

std::string clpl::generateDeclarations(const SList &l) {
    std::string out {"// GENERATED FILE\n"};
    for (const auto &st : l) {
        if (instanceof<FuncDeclStmt>(st)) {
            auto fn = downcast<FuncDeclStmt>(st);
            out += "func " + fn->name.identName + "(";
            for (unsigned int i = 0; i < fn->params.size(); i++) {
                auto &param = fn->params[i];
                out += param.name.identName + ":" + param.type->toString();
                if (i + 1 < fn->params.size()) out += ",";
            }
            out += ")->" + fn->type->toString() + ";\n";
        }

        if (instanceof<VarDeclStmt>(st)) {
            auto vdc = downcast<VarDeclStmt>(st);
            out += "var " + vdc->name.identName + ":" + vdc->type->toString() + ";\n";
        }
    }
    return out;
}