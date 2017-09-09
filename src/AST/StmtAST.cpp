// Created by aritz.

#include <sstream>
#include "CodegenHelper.h"
#include "StmtAST.h"

AssignStmtAST::AssignStmtAST(std::unique_ptr<VarDeclStmtAST> decl, std::string lvar, std::unique_ptr<ExprAST> value)
        : decl(std::move(decl)), lvar(std::move(lvar)), value(std::move(value)) {}

std::string AssignStmtAST::print() {
    return (this->decl ? TypeToString(this->decl->type) + " " : "") + this->lvar + " = " + this->value->print() + ";\n";
}

void AssignStmtAST::generate() {
    llvm::AllocaInst *alloca;
    if (decl) {
        decl->generate();
    }
    alloca = FindAlloca(lvar);

    if (alloca == nullptr)
        return;

    llvm::Value *pValue = this->value->generate();
    Builder.CreateStore(pValue, alloca);
}

VarDeclStmtAST *AssignStmtAST::GetDecl() {
    return this->decl.get();
}

std::string AssignStmtAST::GetLVar() {
    return this->lvar;
}

ExprAST *AssignStmtAST::GetValue() {
    return this->value.get();
}

IfStmtAST::IfStmtAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<StmtAST> tru, std::unique_ptr<StmtAST> fals)
        : cond(std::move(cond)), tru(std::move(tru)), fals(std::move(fals)) {}

std::string IfStmtAST::print() {
    std::stringstream ss;
    ss << "if (" << this->cond->print() << ") {\n";
    ss << this->tru->print();
    ss << "}";
    if (this->fals) {
        ss << " else {\n";
        ss << this->fals->print();
        ss << "}\n";
    } else {
        ss << "\n";
    }
    return ss.str();
}

void IfStmtAST::generate() {
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(TheContext, "then", TheFunction);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(TheContext, "else");
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(TheContext, "ifcont");

    llvm::Value *condV = this->cond->generate();

    Builder.CreateCondBr(condV, thenBB, elseBB);
    bool jumpGenerated = false;

    Builder.SetInsertPoint(thenBB);
    this->tru->generate();
    if (!TheFunction->back().back().isTerminator()) {
        jumpGenerated = true;
        Builder.CreateBr(mergeBB);
    }

    TheFunction->getBasicBlockList().push_back(elseBB);

    Builder.SetInsertPoint(elseBB);
    this->fals->generate();
    if (!TheFunction->back().back().isTerminator()) {
        jumpGenerated = true;
        Builder.CreateBr(mergeBB);
    }

    if (jumpGenerated) {
        TheFunction->getBasicBlockList().push_back(mergeBB);
        Builder.SetInsertPoint(mergeBB);
    } else {
        delete mergeBB;
    }
}

ExprAST *IfStmtAST::GetCond() {
    return this->cond.get();
}

StmtAST *IfStmtAST::GetTrue() {
    return this->tru.get();
}

StmtAST *IfStmtAST::GetFalse() {
    return this->fals.get();
}

WhileStmtAST::WhileStmtAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<StmtAST> body)
        : cond(std::move(cond)), body(std::move(body)) {}

std::string WhileStmtAST::print() {
    std::stringstream ss;
    ss << "while (" << this->cond->print() << ") {\n";
    ss << this->body->print();
    ss << "}\n";
    return ss.str();
}

void WhileStmtAST::generate() {
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(TheContext, "whilecond", TheFunction);
    llvm::BasicBlock *whileBB = llvm::BasicBlock::Create(TheContext, "while");
    llvm::BasicBlock *contBB = llvm::BasicBlock::Create(TheContext, "whilecont");

    Builder.CreateBr(condBB); // Previous block will jump to condition block

    Builder.SetInsertPoint(condBB);

    llvm::Value *condV = this->cond->generate();
    Builder.CreateCondBr(condV, whileBB, contBB);

    TheFunction->getBasicBlockList().push_back(whileBB);
    Builder.SetInsertPoint(whileBB);

    this->body->generate();
    if (!TheFunction->back().back().isTerminator()) Builder.CreateBr(condBB);

    TheFunction->getBasicBlockList().push_back(contBB);
    Builder.SetInsertPoint(contBB);
}

ExprAST *WhileStmtAST::GetCond() {
    return this->cond.get();
}

StmtAST *WhileStmtAST::GetBody() {
    return this->body.get();
}

ReturnStmtAST::ReturnStmtAST(std::unique_ptr<ExprAST> value)
        : value(std::move(value)) {}

std::string ReturnStmtAST::print() {
    return "return " + this->value->print() + ";\n";
}

void ReturnStmtAST::generate() {
    if (this->value == nullptr) Builder.CreateRetVoid();
    else Builder.CreateRet(this->value->generate());
}

ExprAST *ReturnStmtAST::GetExpr() {
    return this->value.get();
}

ExprStmtAST::ExprStmtAST(std::unique_ptr<ExprAST> expr)
        : expr(std::move(expr)) {}

std::string ExprStmtAST::print() {
    return this->expr->print() + ";\n";
}

void ExprStmtAST::generate() {
    this->expr->generate();
}

ExprAST *ExprStmtAST::GetExpr() {
    return this->expr.get();
}

BlockStmtAST::BlockStmtAST(std::vector<std::unique_ptr<StmtAST>> stmts)
        : stmts(std::move(stmts)) {}

std::string BlockStmtAST::print() {
    std::stringstream ss;

    for (auto &s : this->stmts) {
        ss << s->print() << "\n";
    }
    return ss.str();
}

void BlockStmtAST::generate() {
    for (auto &s : this->stmts) {
        s->generate();
    }
}

const std::vector<std::unique_ptr<StmtAST>> &BlockStmtAST::GetStmts() {
    return this->stmts;
}

VarDeclStmtAST::VarDeclStmtAST(std::string name, Type type)
        : name(std::move(name)), type(type) {}

void VarDeclStmtAST::generate() {
    CreateAlloca(this->type, name);
}

std::string VarDeclStmtAST::print() {
    return TypeToString(this->type) + " " + this->name + ";\n";
}

std::string VarDeclStmtAST::GetName() {
    return this->name;
}

Type VarDeclStmtAST::GetType() {
    return this->type;
}
