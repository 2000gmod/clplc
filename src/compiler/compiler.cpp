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
    mod.dump();
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
    globals.insert({{funcs->name.identName, func}});
    if (funcs->body == nullptr) return;


    for (size_t i = 0; i < func->arg_size(); i++) {
        auto name = funcs->params[i].name.identName;
        auto *val = func->getArg(i);
        arguments.insert({{name, val}});
    }

    auto *entry = BasicBlock::Create(context, "", func);
    returnBlock = BasicBlock::Create(context, "", func);
    builder.SetInsertPoint(entry);

    if (!rtype->isVoidTy()) {
        returnValue = builder.CreateAlloca(rtype, ConstantInt::get(typemap.at("i32"), 1));
    }

    isOnGlobalScope = false;
    parent = func;
    compileBlock(funcs->body);
    builder.CreateBr(returnBlock);

    builder.SetInsertPoint(returnBlock);
    if (!rtype->isVoidTy()) {
        auto *val = builder.CreateLoad(rtype, returnValue);
        builder.CreateRet(val);
    }
    else {
        builder.CreateRetVoid();
    }

    parent = nullptr;
    isOnGlobalScope = true;
    localvars.clear();
    arguments.clear();
}

void Compiler::compileVarDecl(const StmtSP &s) {
    auto vards = downcast<VarDeclStmt>(s);

    auto size = ConstantInt::get(builder.getInt32Ty(), 1);
    auto var = builder.CreateAlloca(getType(vards->type), size);

    if (isOnGlobalScope) globals.insert_or_assign(vards->name.identName, var);
    else localvars.insert_or_assign(vards->name.identName, var);

    if (vards->value != nullptr) {
        builder.CreateStore(compileExpression(vards->value), var);
    }
}

void Compiler::compileReturn(const StmtSP &s) {
    auto rets = downcast<ReturnStmt>(s);
    if (rets->value != nullptr) {
        builder.CreateStore(compileExpression(rets->value), returnValue);
    }
    builder.CreateBr(returnBlock);
}

void Compiler::compileIf(const StmtSP &s) {
    auto ifs = downcast<IfStmt>(s);
    auto *condition = compileExpression(ifs->condition);
    auto *currentBlock = builder.GetInsertBlock();

    auto *ifBranch = BasicBlock::Create(context, "", parent);
    auto *elseBranch = (ifs->elseBody == nullptr) ? nullptr : BasicBlock::Create(context, "", parent);
    auto *exitBlock = BasicBlock::Create(context, "", parent);

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

    auto *condBlock = BasicBlock::Create(context, "", parent);
    auto *loopBlock = BasicBlock::Create(context, "", parent);
    auto *exitBlock = BasicBlock::Create(context, "", parent);

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

    auto *condBlock = BasicBlock::Create(context, "", parent);
    auto *loopBlock = BasicBlock::Create(context, "", parent);
    auto *exitBlock = BasicBlock::Create(context, "", parent);

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
    auto *brb = BasicBlock::Create(context, "", parent);
    builder.SetInsertPoint(current);

    builder.CreateBr(brb);
    builder.SetInsertPoint(brb);
    builder.CreateBr(innermostExit);

    auto *next = BasicBlock::Create(context, "", parent);
    builder.SetInsertPoint(next);
}

void Compiler::compileContinue(const StmtSP &s) {
    auto *current = builder.GetInsertBlock();
    auto *brb = BasicBlock::Create(context, "", parent);
    builder.SetInsertPoint(current);

    builder.CreateBr(brb);
    builder.SetInsertPoint(brb);
    builder.CreateBr(innermostCondition);

    auto *next = BasicBlock::Create(context, "", parent);
    builder.SetInsertPoint(next);
}

Value *Compiler::compileExpression(const ExprSP &expr, bool isLvalue) {
    if (instanceof<LiteralExpr>(expr)) return compileLiteral(expr);
    else if (instanceof<IdentifierExpr>(expr)) return compileIdent(expr, isLvalue);
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

Value *Compiler::compileIdent(const ExprSP &expr, bool isLvalue) {
    auto iexp = downcast<IdentifierExpr>(expr);

    if (localvars.contains(iexp->ident.identName)) {
        if (isLvalue) return localvars.at(iexp->ident.identName);
        else return builder.CreateLoad(getType(iexp->type), localvars.at(iexp->ident.identName));
    }
    else if (globals.contains(iexp->ident.identName)) {
        if (isLvalue) return globals.at(iexp->ident.identName);
        else return builder.CreateLoad(getType(iexp->type), globals.at(iexp->ident.identName));
    }
    else if (arguments.contains(iexp->ident.identName)) {
        return arguments.at(iexp->ident.identName);
    }
    return nullptr;
}

Value *Compiler::compileUnary(const ExprSP &expr) {
    auto uexp = downcast<UnaryExpr>(expr);

    auto *type = getType(uexp->type);
    auto *subexp = compileExpression(uexp->expr);

    switch (uexp->op) {
        case TokenT::MINUS: {
            if (type->isPointerTy()) throw 1;
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

Value *Compiler::compileBinary(const ExprSP &expr) {
    auto bexp = downcast<BinaryExpr>(expr);
    auto *type = getType(bexp->type);
    bool isSigned = bexp->type->isSigned();

    auto *lhs = compileExpression(bexp->left);
    auto *rhs = compileExpression(bexp->right);

    switch (bexp->op) {
        case TokenT::PLUS: {
            if (type->isIntOrPtrTy()) return builder.CreateAdd(lhs, rhs);
            return builder.CreateFAdd(lhs, rhs);
        }
        case TokenT::MINUS: {
            if (type->isIntOrPtrTy()) return builder.CreateSub(lhs, rhs);
            return builder.CreateFSub(lhs, rhs);
        }
        case TokenT::STAR: {
            if (type->isIntegerTy()) return builder.CreateMul(lhs, rhs);
            return builder.CreateFMul(lhs, rhs);
        }
        case TokenT::SLASH: {
            if (type->isIntegerTy()) {
                if (isSigned) return builder.CreateSDiv(lhs, rhs);
                return builder.CreateUDiv(lhs, rhs);
            }
            return builder.CreateFDiv(lhs, rhs);
        }
        case TokenT::MOD: {
            if (type->isIntegerTy()) {
                if (isSigned) return builder.CreateSRem(lhs, rhs);
                return builder.CreateURem(lhs, rhs);
            }
            return builder.CreateFRem(lhs, rhs);
        }
        case TokenT::EQ: {
            if (type->isIntOrPtrTy()) {
                return builder.CreateCmp(CmpInst::Predicate::ICMP_EQ, lhs, rhs);
            }
            return builder.CreateFCmp(CmpInst::Predicate::FCMP_OEQ, lhs, rhs);
        }
        case TokenT::NOT_EQ: {
            if (type->isIntOrPtrTy()) {
                return builder.CreateCmp(CmpInst::Predicate::ICMP_NE, lhs, rhs);
            }
            return builder.CreateFCmp(CmpInst::Predicate::FCMP_ONE, lhs, rhs);
        }
        case TokenT::GT: {
            if (type->isIntOrPtrTy()) {
                if (isSigned) return builder.CreateCmp(CmpInst::Predicate::ICMP_SGT, lhs, rhs);
                return builder.CreateCmp(CmpInst::Predicate::ICMP_UGT, lhs, rhs);
            }
            return builder.CreateFCmp(CmpInst::Predicate::FCMP_OGT, lhs, rhs);
        }
        case TokenT::GEQ: {
            if (type->isIntOrPtrTy()) {
                if (isSigned) return builder.CreateCmp(CmpInst::Predicate::ICMP_SGE, lhs, rhs);
                return builder.CreateCmp(CmpInst::Predicate::ICMP_UGE, lhs, rhs);
            }
            return builder.CreateFCmp(CmpInst::Predicate::FCMP_OGE, lhs, rhs);
        }
        case TokenT::LT: {
            if (type->isIntOrPtrTy()) {
                if (isSigned) return builder.CreateCmp(CmpInst::Predicate::ICMP_SLT, lhs, rhs);
                return builder.CreateCmp(CmpInst::Predicate::ICMP_ULT, lhs, rhs);
            }
            return builder.CreateFCmp(CmpInst::Predicate::FCMP_OLT, lhs, rhs);
        }
        case TokenT::LEQ: {
            if (type->isIntOrPtrTy()) {
                if (isSigned) return builder.CreateCmp(CmpInst::Predicate::ICMP_SLE, lhs, rhs);
                return builder.CreateCmp(CmpInst::Predicate::ICMP_ULE, lhs, rhs);
            }
            return builder.CreateFCmp(CmpInst::Predicate::FCMP_OLE, lhs, rhs);
        }
        case TokenT::AND: {
            return builder.CreateAnd(lhs, rhs);
        }
        case TokenT::OR: {
            return builder.CreateOr(lhs, rhs);
        }
        default:
            return nullptr;
    }
}

Value *Compiler::compileGroup(const ExprSP &expr) {
    auto gexp = downcast<GroupExpr>(expr);

    return compileExpression(gexp->expr);
}

Value *Compiler::compileAssign(const ExprSP &expr) {
    auto aexp = downcast<AssignExpr>(expr);
    auto val = compileExpression(aexp->value);

    return builder.CreateStore(val, compileExpression(aexp->target, true));
}

Value *Compiler::compileCall(const ExprSP &expr) {
    auto cexp = downcast<CallExpr>(expr);
    Value *callee = nullptr;
    if (instanceof<IdentifierExpr>(cexp->callee)) {
        for (auto &f : mod.functions()) {
            if (f.getName() == downcast<IdentifierExpr>(cexp->callee)->ident.identName) {
                callee = &f;
                goto out;
            }
        }
        callee = compileExpression(cexp->callee);
    }
    else {
        callee = compileExpression(cexp->callee);
    }
    out:
    std::vector<llvm::Value*> argvalues;
    std::vector<llvm::Type*> argtypes;
    for (auto &arg : cexp->args) {
        auto *argval = compileExpression(arg);
        argvalues.push_back(argval);
        argtypes.push_back(getType(arg->type));
    }
    auto *ftype = FunctionType::get(getType(cexp->type), argtypes, false);
    return builder.CreateCall(ftype, callee, argvalues);
}