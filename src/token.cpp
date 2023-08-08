#include "token.hpp"

Token::Token() {
    type = TokenT::EOFILE;
    identName = "";
    strValue = "";
    intValue = 0;
    doubleValue = 0.0;
    boolValue = false;
    line = 0;
}

Token::Token(int line) : Token() {
    this->line = line;
}

Token::Token(const Token &other) {
    type = other.type;
    identName = other.identName;
    strValue = other.strValue;
    intValue = other.intValue;
    doubleValue = other.doubleValue;
    boolValue = other.boolValue;
    line = other.line;
}

std::string Token::toString() const {
    std::string out = std::to_string((int) type);

    switch (type) {
        case TokenT::INT_LIT:
            out += std::string(": ") + std::to_string(intValue);
            break;
        case TokenT::DOUBLE_LIT:
            out += std::string(": ") + std::to_string(doubleValue);
            break;
        case TokenT::STRING_LIT:
            out += std::string(": ") + strValue;
            break;
        case TokenT::BOOL_LIT:
            out += std::string(": ") + (boolValue ? "true" : "false");
            break;
        case TokenT::IDENTIFIER:
            out += std::string(": ") + identName;
            break;
        default:
            break;
    }

    return out;
}