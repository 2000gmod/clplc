#pragma once

#include <memory>

#include "token.hpp"

struct Type {
    virtual ~Type() = default;
};

typedef std::shared_ptr<Type> TypeSP;

struct NamedType : public Type {
    Token name;

    NamedType(const Token &name) : name(name) { }
};

typedef std::shared_ptr<NamedType> NamedTypeSP;