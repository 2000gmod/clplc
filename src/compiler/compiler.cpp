#include "compiler.hpp"

using namespace llvm;
using clpl::Compiler;

Compiler::Compiler(const char *fname, const SList &statements) : statements(statements), mod(fname, context), builder(context) {
    typemap = {
        {"void", builder.getVoidTy()},
        {"bool", builder.getInt1Ty()},
        {"i8", builder.getInt8Ty()},
        {"i16", builder.getInt16Ty()},
        {"i32", builder.getInt32Ty()},
        {"i64", builder.getInt64Ty()},
        {"u8", builder.getInt8Ty()},
        {"u16", builder.getInt16Ty()},
        {"u32", builder.getInt32Ty()},
        {"u64", builder.getInt64Ty()},
        {"f32", builder.getFloatTy()},
        {"f64", builder.getDoubleTy()},
        {"ptr", builder.getPtrTy()}
    };
}

llvm::Type *Compiler::getType(const clpl::TypeSP &type) {
    if (instanceof<NamedType>(type)) {
        return typemap.at(downcast<NamedType>(type)->name.identName);
    }
    else if (instanceof<PointerType>(type) || instanceof<FunctionReferenceType>(type)) {
        return typemap.at("ptr");
    }
    throw 1;
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

void Compiler::compileFunction(const StmtSP &s) {
    auto funcs = downcast<FuncDeclStmt>(s);
    auto rtype = getType(funcs->type);

    std::vector<llvm::Type*> paramtypes;
    for (auto &i : funcs->params) {
        paramtypes.push_back(getType(i.type));
    }

    auto ftype = FunctionType::get(rtype, paramtypes, false);
    auto *func = Function::Create(ftype, Function::ExternalLinkage, funcs->name.identName, this->mod);
    if (funcs->body == nullptr) return;

    auto *entry = BasicBlock::Create(context, "", func);
    builder.SetInsertPoint(entry);

    isOnGlobalScope = false;
    compileBlock(funcs->body);
    isOnGlobalScope = true;
    localvars.clear();
}

void Compiler::compileVarDecl(const StmtSP &s) {
    auto vards = downcast<VarDeclStmt>(s);

    auto size = ConstantInt::get(getType(vards->type), 1);
    auto var = builder.CreateAlloca(getType(vards->type), size);

    if (isOnGlobalScope) globals.insert_or_assign(vards->name.identName, var);
    else localvars.insert_or_assign(vards->name.identName, var);

    if (vards->value != nullptr) {
        builder.CreateStore(compileExpression(vards->value), var);
    }
}

void Compiler::compileReturn(const StmtSP &s) {
    auto rets = downcast<ReturnStmt>(s);
    if (rets->value == nullptr) builder.CreateRetVoid();
    else builder.CreateRet(compileExpression(rets->value));
}

void Compiler::compileIf(const StmtSP &s) {
    auto ifs = downcast<IfStmt>(s);
    auto *condition = compileExpression(ifs->condition);
    auto *currentBlock = builder.GetInsertBlock();

    auto *ifBranch = BasicBlock::Create(context);
    auto *elseBranch = (ifs->elseBody == nullptr) ? nullptr : BasicBlock::Create(context);
    auto *exitBlock = BasicBlock::Create(context);

    builder.SetInsertPoint(currentBlock);
    builder.CreateCondBr(condition, ifBranch, (elseBranch == nullptr) ? exitBlock : elseBranch);

    builder.SetInsertPoint(ifBranch);
    compileStatement(ifs->ifBody);
    builder.CreateBr(exitBlock);

    if (elseBranch != nullptr) {
        builder.SetInsertPoint(elseBranch);
        compileStatement(ifs->elseBody);
        builder.CreateBr(exitBlock);
    }

    builder.SetInsertPoint(exitBlock);
}

void Compiler::compileWhile(const StmtSP &s) {
    auto whs = downcast<WhileStmt>(s);
    
    auto *currentBlock = builder.GetInsertBlock();

    auto *condBlock = BasicBlock::Create(context);
    auto *loopBlock = BasicBlock::Create(context);
    auto *exitBlock = BasicBlock::Create(context);

    auto *prevCond = innermostCondition;
    auto *prevExit = innermostExit;

    innermostCondition = condBlock;
    innermostExit = exitBlock;

    builder.SetInsertPoint(currentBlock);
    builder.CreateBr(condBlock);
    
    builder.SetInsertPoint(condBlock);
    auto *condition = compileExpression(whs->condition);
    builder.CreateCondBr(condition, loopBlock, exitBlock);
    
    builder.SetInsertPoint(loopBlock);
    compileStatement(whs->body);
    builder.CreateBr(condBlock);

    builder.SetInsertPoint(exitBlock);

    innermostCondition = prevCond;
    innermostExit = prevExit;
}

void Compiler::compileFor(const StmtSP &s) {
    auto fors = downcast<ForStmt>(s);

    auto *currentBlock = builder.GetInsertBlock();

    auto *condBlock = BasicBlock::Create(context);
    auto *loopBlock = BasicBlock::Create(context);
    auto *exitBlock = BasicBlock::Create(context);

    auto *prevCond = innermostCondition;
    auto *prevExit = innermostExit;

    innermostCondition = condBlock;
    innermostExit = exitBlock;

    builder.SetInsertPoint(currentBlock);
    compileStatement(fors->init);
    builder.CreateBr(condBlock);

    builder.SetInsertPoint(condBlock);
    auto *condition = compileExpression(fors->condition);
    builder.CreateCondBr(condition, loopBlock, exitBlock);

    builder.SetInsertPoint(loopBlock);
    compileStatement(fors->body);
    compileExpression(fors->increment);
    builder.CreateBr(condBlock);

    builder.SetInsertPoint(exitBlock);

    innermostCondition = prevCond;
    innermostExit = prevExit;
}

void Compiler::compileBreak(const StmtSP &s) {
    auto *current = builder.GetInsertBlock();
    auto *brb = BasicBlock::Create(context);
    builder.SetInsertPoint(current);

    builder.CreateBr(brb);
    builder.SetInsertPoint(brb);
    builder.CreateBr(innermostExit);

    auto *next = BasicBlock::Create(context);
    builder.SetInsertPoint(next);
}

void Compiler::compileContinue(const StmtSP &s) {
    auto *current = builder.GetInsertBlock();
    auto *brb = BasicBlock::Create(context);
    builder.SetInsertPoint(current);

    builder.CreateBr(brb);
    builder.SetInsertPoint(brb);
    builder.CreateBr(innermostCondition);

    auto *next = BasicBlock::Create(context);
    builder.SetInsertPoint(next);
}

Value *Compiler::compileExpression(const ExprSP &expr) {
    if (instanceof<LiteralExpr>(expr)) return compileLiteral(expr);
    else if (instanceof<IdentifierExpr>(expr)) return compileIdent(expr);
    else if (instanceof<UnaryExpr>(expr)) return compileUnary(expr);
    else if (instanceof<BinaryExpr>(expr)) return compileBinary(expr);
    else if (instanceof<GroupExpr>(expr)) return compileGroup(expr);
    else if (instanceof<AssignExpr>(expr)) return compileAssign(expr);
    else if (instanceof<CallExpr>(expr)) return compileCall(expr);
    return nullptr;
}


Value *Compiler::compileLiteral(const ExprSP &expr) {
    auto lexp = downcast<LiteralExpr>(expr);
    auto tokt = lexp->val.type;

    switch (tokt) {
        case TokenT::BOOL_LIT:
            return ConstantInt::getBool(getType(lexp->type), lexp->val.boolValue);
        case TokenT::INT_LIT:
            return ConstantInt::get(getType(lexp->type), lexp->val.intValue);
        case TokenT::DOUBLE_LIT:
            return ConstantFP::get(getType(lexp->type), lexp->val.doubleValue);
        case TokenT::STRING_LIT: {
            auto *gv = new GlobalVariable(getType(lexp->type), true, GlobalValue::ExternalLinkage);
            return ConstantExpr::getBitCast(gv, getType(lexp->type));
        }
        default:
            break;
    }
    return nullptr;
}

Value *Compiler::compileIdent(const ExprSP &expr) {
    auto iexp = downcast<IdentifierExpr>(expr);

    if (localvars.contains(iexp->ident.identName)) {
        return builder.CreateLoad(getType(iexp->type), localvars.at(iexp->ident.identName));
    }
    else if (globals.contains(iexp->ident.identName)) {
        return builder.CreateLoad(getType(iexp->type), globals.at(iexp->ident.identName));
    }
    return nullptr;
}

Value *Compiler::compileUnary(const ExprSP &expr) {
    auto uexp = downcast<UnaryExpr>(expr);

    auto *type = getType(uexp->type);
    auto *subexp = compileExpression(uexp->expr);

    switch (uexp->op) {
        case TokenT::MINUS: {
            if (type->isIntegerTy()) return builder.CreateSub(ConstantInt::get(type, 0), subexp);
            return builder.CreateFSub(ConstantFP::get(type, 0), subexp);
        }
        case TokenT::NOT: {
            return builder.CreateNot(subexp);
        }
        default:
            return nullptr;
    }
}