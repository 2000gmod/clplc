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

            llvm::BasicBlock *innermostExit = nullptr, *innermostCondition = nullptr;
            llvm::BasicBlock *returnBlock = nullptr;
            llvm::Value *returnValue;
            llvm::Function *parent = nullptr;

            std::unordered_map<std::string, llvm::Type*> typemap;
            std::unordered_map<std::string, llvm::Value*> globals, localvars, arguments;
            bool isOnGlobalScope = true;

            llvm::Type *getType(const clpl::TypeSP &type);

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

            llvm::Value *compileExpression(const ExprSP &e, bool isLvalue = false);
            llvm::Value *compileLiteral(const ExprSP &e);
            llvm::Value *compileIdent(const ExprSP &e, bool isLvalue = false);
            llvm::Value *compileUnary(const ExprSP &e);
            llvm::Value *compileBinary(const ExprSP &e);
            llvm::Value *compileGroup(const ExprSP &e);
            llvm::Value *compileAssign(const ExprSP &e);
            llvm::Value *compileCall(const ExprSP &e);
    };
}