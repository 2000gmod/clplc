#pragma once

#include <vector>
#include "expression.hpp"
#include "type.hpp"

namespace clpl {
    struct Stmt {
        virtual ~Stmt() = default;
    };

    typedef std::shared_ptr<Stmt> StmtSP;

    struct BlockStmt : public Stmt {
        std::vector<StmtSP> statements;
        BlockStmt() = default;
        explicit BlockStmt(const std::vector<StmtSP> &statements) : statements(statements) { }
    };

    typedef std::shared_ptr<BlockStmt> BlockStmtSP;

    struct ExprStmt : public Stmt {
        ExprSP expr;
        ExprStmt() = default;
        explicit ExprStmt(const ExprSP &expr) : expr(expr) { }
    };

    typedef std::shared_ptr<ExprStmt> ExprStmtSP;

    struct ParameterT {
        TypeSP type;
        Token name;
    };

    struct FuncDeclStmt : public Stmt {
        TypeSP type;
        Token name;
        std::vector<ParameterT> params;
        StmtSP body;

        FuncDeclStmt(
            TypeSP type,
            const Token &name, 
            const std::vector<ParameterT> &params, 
            StmtSP body)
            :
                type(std::move(type)),
                name(name),
                params(params),
                body(std::move(body))
        { }

        TypeSP getFuncReferenceType() const {
            std::vector<TypeSP> argTypes;
            for (auto &i : params) argTypes.push_back(i.type);
            return std::make_shared<FunctionReferenceType>(type, argTypes);
        }
    };

    typedef std::shared_ptr<FuncDeclStmt> FuncDeclStmtSP;

    struct VarDeclStmt : public Stmt {
        TypeSP type;
        Token name;
        ExprSP value;
        VarDeclStmt() = default;
        VarDeclStmt(TypeSP type, const Token &name, ExprSP value) : type(std::move(type)), name(name), value(std::move(value)) { }
    };

    typedef std::shared_ptr<VarDeclStmt> VarDeclStmtSP;

    struct ReturnStmt : public Stmt {
        ExprSP value;
        ReturnStmt() = default;
        explicit ReturnStmt(ExprSP value) : value(std::move(value)) { }
    };

    typedef std::shared_ptr<ReturnStmt> ReturnStmtSP;

    struct IfStmt : public Stmt {
        ExprSP condition;
        StmtSP ifBody, elseBody;

        IfStmt() = default;
        IfStmt(ExprSP condition, StmtSP ifBody, StmtSP elseBody)
            :
                condition(std::move(condition)),
                ifBody(std::move(ifBody)),
                elseBody(std::move(elseBody))
        { }
    };

    typedef std::shared_ptr<IfStmt> IfStmtSP;

    struct WhileStmt : public Stmt {
        ExprSP condition;
        StmtSP body;

        WhileStmt() = default;
        WhileStmt(ExprSP condition, StmtSP body) : condition(std::move(condition)), body(std::move(body)) { }
    };

    typedef std::shared_ptr<WhileStmt> WhileStmtSP;

    struct ForStmt : public Stmt {
        StmtSP init;
        ExprSP condition, increment;
        StmtSP body;

        ForStmt() = default;
        ForStmt(StmtSP init, ExprSP condition, ExprSP increment, StmtSP body)
            :
                init(std::move(init)),
                condition(std::move(condition)),
                increment(std::move(increment)),
                body(std::move(body))
        { }
    };

    typedef std::shared_ptr<ForStmt> ForStmtSP;

    struct BreakStmt : public Stmt { };

    typedef std::shared_ptr<BreakStmt> BreakStmtSP;

    struct ContinueStmt : public Stmt { };

    typedef std::shared_ptr<ContinueStmt> ContinueStmtSP;
}
