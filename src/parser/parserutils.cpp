#include "parser.hpp"

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
    auto message = "(at token: " + tok.toString() + ") ";
    message = "ERROR: (at line " + std::to_string(tok.line) + ")" + message + msg; 
    return {message};
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