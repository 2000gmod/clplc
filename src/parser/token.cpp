#include "token.hpp"

using namespace clpl;

const char *toString(TokenT tokt);

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
    std::string out = ::toString(type);

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

const char *toString(TokenT tokt) {
    using enum TokenT;
    switch (tokt) {
        case PLUS:
            return "PLUS";
        case MINUS:
            return "MINUS";
        case STAR:
            return "STAR";
        case SLASH:
            return "SLASH";
        case COMMA:
            return "COMMA";
        case DOT:
            return "DOT";
        case MOD:
            return "MOD";
        case COLON:
            return "COLON";
        case DCOLON:
            return "DOUBLE_COLON";
        case ARROW:
            return "ARROW";
        case ASSIGN:
            return "ASSIGN";
        case EQ:
            return "EQUAL";
        case NOT_EQ:
            return "NOT_EQUAL";
        case LT:
            return "LESS_THAN";
        case GT:
            return "GREATER_THAN";
        case LEQ:
            return "LESS_OR_EQUAL";
        case GEQ:
            return "GREATER_OR_EQUAL";
        case SEMICOLON:
            return "SEMICOLON";
        case AND:
            return "AND";
        case NOT:
            return "NOT";
        case OR:
            return "OR";
        case LEFT_PAREN:
            return "LEFT_PARENTHESIS";
        case RIGHT_PAREN:
            return "RIGHT_PARENTHESIS";
        case LEFT_SQR:
            return "LEFT_SQUARE_BRACKET";
        case RIGHT_SQR:
            return "RIGHT_SQUARE_BRACKET";
        case LEFT_CUR:
            return "LEFT_CURLY_BRACKET";
        case RIGHT_CUR:
            return "RIGHT_CURLY_BRACKET";
        case IF:
            return "IF";
        case ELSE:
            return "ELSE";
        case FOR:
            return "FOR";
        case WHILE:
            return "WHILE";
        case BREAK:
            return "BREAK";
        case CONTINUE:
            return "CONTINUE";
        case RETURN:
            return "RETURN";
        case VAR:
            return "VAR";
        case FUNC:
            return "FUNC";
        case METHOD:
            return "METHOD";
        case OPERATOR:
            return "OPERATOR";
        case IMPORT:
            return "IMPORT";
        case INT_LIT:
            return "INT_LITERAL";
        case DOUBLE_LIT:
            return "DOUBLE_LITERAL";
        case BOOL_LIT:
            return "BOOL_LITERAL";
        case STRING_LIT:
            return "STRING_LITERAL";
        case IDENTIFIER:
            return "IDENTIFIER";
        case ERROR:
            return "ERROR";
        case EOFILE:
            return "EOFILE";
        default:
            return "UNKNOWN_TOKEN";
    }
}