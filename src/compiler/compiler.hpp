#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "../parser/parser.hpp"

namespace clpl {
    class Compiler {
        private:
            SList statements;
            llvm::LLVMContext context;
            llvm::Module mod;
            llvm::IRBuilder<> builder;

            llvm::BasicBlock *innermostLoop = nullptr;

        public:
            Compiler(const char *fname, const SList &statements);
            void output(const char *outpath);
            void compile();

        private:
            void compileStatement(const StmtSP &s);
            void compileBlock(const StmtSP &s);
            void compileExprStmt(const StmtSP &s);
            void compileFunction(const StmtSP &s);
            void compileVarDecl(const StmtSP &s);
            void compileReturn(const StmtSP &s);
            void compileIf(const StmtSP &s);
            void compileWhile(const StmtSP &s);
            void compileFor(const StmtSP &s);
            void compileBreak(const StmtSP &s);
            void compileContinue(const StmtSP &s);

            void compileExpression(const ExprSP &e);
            void compileLiteral(const ExprSP &e);
            void compileIdent(const ExprSP &e);
            void compileUnary(const ExprSP &e);
            void compileBinary(const ExprSP &e);
            void compileGroup(const ExprSP &e);
            void compileAssign(const ExprSP &e);
            void compileCall(const ExprSP &e);
    };
}