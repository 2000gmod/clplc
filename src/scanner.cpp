#include "scanner.hpp"

Scanner::Scanner(const std::string &src) {
    this->src = src;

    keywords = {
        {"and", TokenT::AND},
        {"or", TokenT::OR},
        {"not", TokenT::NOT},

        {"true", TokenT::BOOL_LIT},
        {"false", TokenT::BOOL_LIT},

        {"if", TokenT::IF},
        {"else", TokenT::ELSE},
        {"for", TokenT::FOR},
        {"while", TokenT::WHILE},
        {"break", TokenT::BREAK},
        {"continue", TokenT::CONTINUE},
        {"return", TokenT::RETURN},

        {"var", TokenT::VAR},
        {"fn", TokenT::FN},
        {"method", TokenT::METHOD},
        {"operator", TokenT::OPERATOR},
        {"import", TokenT::IMPORT},
    };
}

std::vector<Token> Scanner::tokenize() {
    while (!atEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back(Token(line));
    return tokens;
}

void Scanner::scanToken() {
    char c = advance();

    switch (c) {
        case '(':
            addToken(TokenT::LEFT_PAREN);
            break;
        case ')':
            addToken(TokenT::RIGHT_PAREN);
            break;
        case '{':
            addToken(TokenT::LEFT_CUR);
            break;
        case '}':
            addToken(TokenT::RIGHT_CUR);
            break;
        case '[':
            addToken(TokenT::LEFT_SQR);
            break;
        case ']':
            addToken(TokenT::RIGHT_SQR);
            break;
        
        case '+':
            addToken(TokenT::PLUS);
            break;
        case '-':
            if (match('>')) addToken(TokenT::ARROW);
            else addToken(TokenT::MINUS);
            break;
        case '*':
            addToken(TokenT::STAR);
            break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !atEnd()) advance();
            }
            else addToken(TokenT::SLASH);
            break;
        case ',':
            addToken(TokenT::COMMA);
            break;
        case '.':
            addToken(TokenT::DOT);
            break;
        case '%':
            addToken(TokenT::MOD);
            break;
        case ':':
            if (match(':')) addToken(TokenT::DCOLON);
            else addToken(TokenT::COLON);
            break;
        case ';':
            addToken(TokenT::SEMICOLON);
            break;
        case '=':
            addToken(match('=') ? TokenT::EQ : TokenT::ASSIGN);
            break;
        case '>':
            addToken(match('=') ? TokenT::GEQ : TokenT::GT);
            break;
        case '<':
            addToken(match('=') ? TokenT::LEQ : TokenT::LT);
            break;
        case '!':
            addToken(match('=') ? TokenT::NOT_EQ : TokenT::NOT);
            break;
        
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            break;
        case '"':
            scanString();
            break;
        
        default:
            if (isDigit(c)) scanNumber();
            else if (isAlpha(c)) scanIdentifier();
            else throw c;
            break;
    }
}

bool Scanner::match(char expected) {
    if (atEnd()) return false;
    if (src[current] != expected) return false;
    current++;
    return true;
}

char Scanner::peek() {
    if (atEnd()) return '\0';
    return src[current];
}

char Scanner::peekNext() {
    if (current + 1 >= (int) src.length()) return '\0';
    return src[current + 1];
}

bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Scanner::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

char Scanner::advance() {
    current++;
    return src[current - 1];
}

bool Scanner::atEnd() {
    return current >= (int) src.length();
}

void Scanner::addToken(const TokenT &tokt) {
    Token tok(line);
    tok.type = tokt;
    tokens.push_back(tok);
}

void Scanner::addToken(const Token &tok) {
    tokens.push_back(tok);
}

void Scanner::scanIdentifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = src.substr(start, current - start);
    TokenT type = TokenT::ERROR;

    if (keywords.count(text) == 0) {
        Token tok(line);
        tok.type = TokenT::IDENTIFIER;
        tok.identName = text;
        addToken(tok);
        return;
    }
    else {
        Token tok(line);
        tok.type = keywords[text];
        if (tok.type == TokenT::BOOL_LIT) {
            tok.boolValue = text == "true";
        }
        addToken(tok);
        return;
    }

    addToken(type);
}

void Scanner::scanNumber() {
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();

        Token tok(line);
        tok.type = TokenT::DOUBLE_LIT;
        tok.doubleValue = std::stod(src.substr(start, current - start));
        addToken(tok);
        return;
    }

    Token tok(line);
    tok.type = TokenT::INT_LIT;
    tok.intValue = std::stoi(src.substr(start, current - start));
    addToken(tok);
    return;
}

void Scanner::scanString() {
    while (peek() != '"' && !atEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (atEnd()) {
        throw 2;
    }

    advance();

    auto val = src.substr(start + 1, current - start - 2);
    val = formatEscapes(val);

    Token tok(line);
    tok.type = TokenT::STRING_LIT;
    tok.strValue = val;
    addToken(tok);
}

std::string Scanner::formatEscapes(const std::string &src) {
    std::string res = "";
    for (int i = 0; i < (int) src.length(); i++) {
        if (src[i] == '\\' && i != (int) src.length() - 1) {
            switch (src[i + 1]) {
                case 'n':
                    res += "\n";
                    i += 1;
                    break;
                case 't':
                    res += "\t";
                    i += 1;
                    break;
                default:
                    break;
            }
        }
        else res += src[i];
    }
    return res;
}
