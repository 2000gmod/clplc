#pragma once

#include <vector>
#include "expression.hpp"
#include "type.hpp"

struct Stmt {
    virtual ~Stmt() = default;
};

typedef std::shared_ptr<Stmt> StmtSP;

struct BlockStmt : public Stmt {
    std::vector<StmtSP> statements;

    BlockStmt(const std::vector<StmtSP> &statements) : statements(statements) { }
};

typedef std::shared_ptr<BlockStmt> BlockStmtSP;

struct ExprStmt : public Stmt {
    ExprSP expr;

    ExprStmt(const ExprSP &expr) : expr(expr) { }
};

typedef std::shared_ptr<ExprStmt> ExprStmtSP;

struct ParameterT {
    TypeSP type;
    Token name;
};

enum class FunctionClass {
    FPROC,
    FMETHOD,
    FOPERATOR
};

struct FuncDeclStmt : public Stmt {
    TypeSP type;
    Token name;
    std::vector<ParameterT> params;
    StmtSP body;
    FunctionClass fnclass;

    FuncDeclStmt(
        const TypeSP &type, 
        const Token &name, 
        const std::vector<ParameterT> &params, 
        const StmtSP &body, 
        const FunctionClass fnclass)
        :
            type(type),
            name(name),
            params(params),
            body(body),
            fnclass(fnclass)
    { }
};

typedef std::shared_ptr<FuncDeclStmt> FuncDeclStmtSP;

struct VarDeclStmt : public Stmt {
    TypeSP type;
    Token name;
    ExprSP value;

    VarDeclStmt(const TypeSP &type, const Token &name, const ExprSP &value) : type(type), name(name), value(value) { }
};

typedef std::shared_ptr<VarDeclStmt> VarDeclStmtSP;

struct ReturnStmt : public Stmt {
    ExprSP value;

    ReturnStmt(const ExprSP &value) : value(value) { }
};

typedef std::shared_ptr<ReturnStmt> ReturnStmtSP;

struct IfStmt : public Stmt {
    ExprSP condition;
    StmtSP ifBody, elseBody;

    IfStmt(const ExprSP &condition, const StmtSP &ifBody, const StmtSP &elseBody) 
        :
            condition(condition), 
            ifBody(ifBody), 
            elseBody(elseBody) 
    { }
};

typedef std::shared_ptr<IfStmt> IfStmtSP;

struct WhileStmt : public Stmt {
    ExprSP condition;
    StmtSP body;

    WhileStmt(const ExprSP &condition, const StmtSP &body) : condition(condition), body(body) { }
};

typedef std::shared_ptr<WhileStmt> WhileStmtSP;

struct ForStmt : public Stmt {
    StmtSP init;
    ExprSP condition, increment;
    StmtSP body;

    ForStmt(const StmtSP &init, const ExprSP &condition, const ExprSP &increment, const StmtSP &body) 
        :
            init(init),
            condition(condition),
            increment(increment),
            body(body)
    { }
};

typedef std::shared_ptr<ForStmt> ForStmtSP;

struct BreakStmt : public Stmt { };

typedef std::shared_ptr<BreakStmt> BreakStmtSP;

struct ContinueStmt : public Stmt { };

typedef std::shared_ptr<ContinueStmt> ContinueStmtSP;
