#pragma once

#include <memory>

#include "token.hpp"

struct Type {
    virtual ~Type() = default;
    virtual std::string toString() const = 0;
};

typedef std::shared_ptr<Type> TypeSP;

struct NamedType : public Type {
    Token name;

    NamedType(const Token &name) : name(name) { }
    NamedType(const std::string &namestr) {
        name.type = TokenT::IDENTIFIER;
        name.identName = namestr;
    }
    virtual std::string toString() const override;
};

typedef std::shared_ptr<NamedType> NamedTypeSP;