#pragma once

#include <memory>
#include <vector>

#include "token.hpp"

namespace clpl {
    struct Type {
        virtual ~Type() = default;
        virtual std::string toString() const = 0;
    };

    bool operator ==(const Type &lhs, const Type &rhs);

    typedef std::shared_ptr<Type> TypeSP;

    struct NamedType : public Type {
        Token name;

        explicit NamedType(const Token &name) : name(name) { }

        explicit NamedType(const std::string &namestr) {
            name.type = TokenT::IDENTIFIER;
            name.identName = namestr;
        }

        std::string toString() const override;
    };

    typedef std::shared_ptr<NamedType> NamedTypeSP;

    struct PointerType : public Type {
        TypeSP dataType;
        ~PointerType() override = default;
        virtual std::string toString() const = 0;
        explicit PointerType(const TypeSP &data) : dataType(data) { }
    };

    typedef std::shared_ptr<PointerType> PointerTypeSP;

    struct IndexedPointerType : public PointerType {
        explicit IndexedPointerType(const TypeSP &type) : PointerType(type) { }
        std::string toString() const override;
    };

    typedef std::shared_ptr<IndexedPointerType> IndexedPointerTypeSP;

    struct ReferencePointerType : public PointerType {
        explicit ReferencePointerType(const TypeSP &type) : PointerType(type) { }
        std::string toString() const override;
    };

    typedef std::shared_ptr<ReferencePointerType> ReferencePointerTypeSP;

    struct FunctionReferenceType : public Type {
        TypeSP returnType;
        std::vector<TypeSP> argTypes;

        FunctionReferenceType(TypeSP rt, const std::vector<TypeSP> &ats) : returnType(std::move(rt)), argTypes(ats) { }
        std::string toString() const override;
    };

    typedef std::shared_ptr<FunctionReferenceType> FunctionReferenceTypeSP;
}