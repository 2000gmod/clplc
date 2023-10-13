#pragma once

#include "type.hpp"

namespace clpl {
    struct Expr {
        TypeSP type = nullptr;
        virtual ~Expr() = default;
    };

    typedef std::shared_ptr<Expr> ExprSP;

    struct LiteralExpr : public Expr {
        Token val;

        LiteralExpr(const Token &val) : val(val) { }
    };

    typedef std::shared_ptr<LiteralExpr> LiteralExprSP;

    struct IdentifierExpr : public Expr {
        Token ident;

        IdentifierExpr(const Token &ident) : ident(ident) { }
    };

    typedef std::shared_ptr<IdentifierExpr> IdentifierExprSP;

    struct UnaryExpr : public Expr {
        ExprSP expr;
        TokenT op;

        UnaryExpr(const ExprSP &expr, TokenT op) : expr(expr), op(op) { }
    };

    typedef std::shared_ptr<UnaryExpr> UnaryExprSP;

    struct BinaryExpr : public Expr {
        ExprSP left, right;
        TokenT op;

        BinaryExpr(const ExprSP &left, const ExprSP &right, TokenT op) : left(left), right(right), op(op) { }
    };

    typedef std::shared_ptr<BinaryExpr> BinaryExprSP;

    struct GroupExpr : public Expr {
        ExprSP expr;

        GroupExpr(const ExprSP &expr) : expr(expr) { }
    };

    typedef std::shared_ptr<GroupExpr> GroupExprSP;

    struct AssignExpr : public Expr {
        ExprSP target;
        ExprSP value;

        AssignExpr(const ExprSP &target, const ExprSP &value) : target(target), value(value) { }
    };

    typedef std::shared_ptr<AssignExpr> AssignExprSP;

    struct CallExpr : public Expr {
        ExprSP callee;
        std::vector<ExprSP> args;

        CallExpr(const ExprSP &callee, const std::vector<ExprSP> &args) : callee(callee), args(args) { }
    };
}