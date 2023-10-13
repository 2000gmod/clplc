#pragma once

#include <memory>

#include "token.hpp"

namespace clpl {
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

    struct PointerType : public Type {
        TypeSP dataType;
        virtual ~PointerType() = default;
        virtual std::string toString() const = 0;
        PointerType(const TypeSP &data) : dataType(data) { }
    };

    typedef std::shared_ptr<PointerType> PointerTypeSP;

    struct IndexedPointerType : public PointerType {
        IndexedPointerType(const TypeSP &type) : PointerType(type) { }
        virtual std::string toString() const override;
    };

    typedef std::shared_ptr<IndexedPointerType> IndexedPointerTypeSP;

    struct ReferencePointerType : public PointerType {
        ReferencePointerType(const TypeSP &type) : PointerType(type) { }
        virtual std::string toString() const override;
    };

    typedef std::shared_ptr<ReferencePointerType> ReferencePointerTypeSP;
}