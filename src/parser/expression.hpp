#pragma once

#include <utility>

#include "type.hpp"

namespace clpl {
    struct Expr {
        TypeSP type = nullptr;
        virtual ~Expr() = default;
    };

    typedef std::shared_ptr<Expr> ExprSP;

    struct LiteralExpr : public Expr {
        Token val;

        explicit LiteralExpr(const Token &val) : val(val) { }
    };

    typedef std::shared_ptr<LiteralExpr> LiteralExprSP;

    struct IdentifierExpr : public Expr {
        Token ident;

        explicit IdentifierExpr(const Token &ident) : ident(ident) { }
    };

    typedef std::shared_ptr<IdentifierExpr> IdentifierExprSP;

    struct UnaryExpr : public Expr {
        ExprSP expr;
        TokenT op;

        UnaryExpr(ExprSP expr, TokenT op) : expr(std::move(expr)), op(op) { }
    };

    typedef std::shared_ptr<UnaryExpr> UnaryExprSP;

    struct BinaryExpr : public Expr {
        ExprSP left, right;
        TokenT op;

        BinaryExpr(ExprSP left, ExprSP right, TokenT op) : left(std::move(left)), right(std::move(right)), op(op) { }
    };

    typedef std::shared_ptr<BinaryExpr> BinaryExprSP;

    struct GroupExpr : public Expr {
        ExprSP expr;

        explicit GroupExpr(ExprSP expr) : expr(std::move(expr)) { }
    };

    typedef std::shared_ptr<GroupExpr> GroupExprSP;

    struct AssignExpr : public Expr {
        ExprSP target;
        ExprSP value;

        AssignExpr(ExprSP target, ExprSP value) : target(std::move(target)), value(std::move(value)) { }
    };

    typedef std::shared_ptr<AssignExpr> AssignExprSP;

    struct CallExpr : public Expr {
        ExprSP callee;
        std::vector<ExprSP> args;

        CallExpr(ExprSP callee, const std::vector<ExprSP> &args) : callee(std::move(callee)), args(args) { }
    };
}