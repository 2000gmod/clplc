#pragma once

#include <string>

enum class TokenT : int {
    PLUS,
    MINUS,
    STAR,
    SLASH,
    COMMA,
    DOT,
    MOD,
    COLON,
    DCOLON,
    ARROW,

    ASSIGN,
    EQ,
    NOT_EQ,
    LT,
    GT,
    LEQ,
    GEQ,
    SEMICOLON,

    AND,
    NOT,
    OR,

    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_SQR,
    RIGHT_SQR,
    LEFT_CUR,
    RIGHT_CUR,

    IF,
    ELSE,
    FOR,
    WHILE,
    BREAK,
    CONTINUE,
    RETURN,
    VAR,
    FN,
    METHOD,
    OPERATOR,

    IMPORT,

    INT_LIT,
    DOUBLE_LIT,
    BOOL_LIT,
    STRING_LIT,
    IDENTIFIER,

    ERROR,
    EOFILE
};

struct Token {
    TokenT type;

    std::string identName;
    
    std::string strValue;
    int intValue;
    double doubleValue;
    bool boolValue;

    int line;

    Token();
    Token(int line);
    Token(const Token &other);

    std::string toString() const;
};