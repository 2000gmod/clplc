#include "compiler.hpp"

using namespace llvm;
using clpl::Compiler;

Compiler::Compiler(const char *fname, const SList &statements) : statements(statements), mod(fname, context), builder(context) {

}

void Compiler::output(const char *outpath) {
    (void) outpath;
}

void Compiler::compile() {
    for (const auto &i : statements) {
        compileStatement(i);
    }
}

void Compiler::compileStatement(const StmtSP &s) {
    if (instanceof<BlockStmt>(s)) compileBlock(s);
    else if (instanceof<ExprStmt>(s)) compileExprStmt(s);
    else if (instanceof<FuncDeclStmt>(s)) compileFunction(s);
    else if (instanceof<VarDeclStmt>(s)) compileVarDecl(s);
    else if (instanceof<ReturnStmt>(s)) compileReturn(s);
    else if (instanceof<IfStmt>(s)) compileIf(s);
    else if (instanceof<WhileStmt>(s)) compileWhile(s);
    else if (instanceof<ForStmt>(s)) compileFor(s);
    else if (instanceof<BreakStmt>(s)) compileBreak(s);
    else if (instanceof<ContinueStmt>(s)) compileContinue(s);
    else throw 1;
}

void Compiler::compileBlock(const StmtSP &s) {
    auto st = downcast<BlockStmt>(s);

    for (const auto &i : st->statements) {
        compileStatement(i);
    }
}

void Compiler::compileExprStmt(const StmtSP &s) {
    auto expr = (downcast<ExprStmt>(s))->expr;
    compileExpression(expr);
}

void Compiler::compileExpression(const ExprSP &expr) {
    if (instanceof<LiteralExpr>(expr)) compileLiteral(expr);
    else if (instanceof<IdentifierExpr>(expr)) compileIdent(expr);
    else if (instanceof<UnaryExpr>(expr)) compileUnary(expr);
    else if (instanceof<BinaryExpr>(expr)) compileBinary(expr);
    else if (instanceof<GroupExpr>(expr)) compileGroup(expr);
    else if (instanceof<AssignExpr>(expr)) compileAssign(expr);
    else if (instanceof<CallExpr>(expr)) compileCall(expr);

}