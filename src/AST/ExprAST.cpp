// Created by aritz.

#include "ExprAST.h"

#include <utility>
#include <sstream>
#include "CodegenHelper.h"

StringExprAST::StringExprAST(std::string value) : value(std::move(value)) {}

std::string StringExprAST::print() {
    return "\"" + this->value + "\"";
}

llvm::Value *StringExprAST::generate() {
    return LogErrorV("Not yet implemented");
}

FloatExprAST::FloatExprAST(float value) : value(value) {}

std::string FloatExprAST::print() {
    return std::to_string(this->value);
}

llvm::Value *FloatExprAST::generate() {
    return llvm::ConstantFP::get(TheContext, llvm::APFloat(this->value));
}

IntExprAST::IntExprAST(int value) : value(value) {}

std::string IntExprAST::print() {
    std::stringstream ss;
    ss << this->value;
    return ss.str();
}

llvm::Value *IntExprAST::generate() {
    return llvm::ConstantInt::get(TheContext, llvm::APInt(32, static_cast<uint64_t>(this->value), true));
}

BinaryExprAST::BinaryExprAST(BinaryOp op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
        : LHS(std::move(LHS)), RHS(std::move(RHS)), op(op) {}

std::string BinaryExprAST::print() {
    std::string oper;
    switch (this->op) {
        case BinaryOp::Add:
            oper = " + ";
            break;
        case BinaryOp::Sub:
            oper = " - ";
            break;
        case BinaryOp::Mul:
            oper = " * ";
            break;
        case BinaryOp::Div:
            oper = " / ";
            break;
    }

    return this->LHS->print() + oper + this->RHS->print();
}

llvm::Value *BinaryExprAST::generate() {
    llvm::Value *L = LHS->generate();
    llvm::Value *R = RHS->generate();
    if (L == nullptr || R == nullptr)
        return nullptr;
    if (L->getType()->isFloatingPointTy()) {
        switch (op) {
            case BinaryOp::Add:
                return Builder.CreateFAdd(L, R, "addtmp");
            case BinaryOp::Sub:
                return Builder.CreateFSub(L, R, "subtmp");
            case BinaryOp::Mul:
                return Builder.CreateFMul(L, R, "multmp");
            case BinaryOp::Div:
                return Builder.CreateFDiv(L, R, "divtmp");
        }
    } else {
        switch (op) {
            case BinaryOp::Add:
                return Builder.CreateAdd(L, R, "addtmp");
            case BinaryOp::Sub:
                return Builder.CreateSub(L, R, "subtmp");
            case BinaryOp::Mul:
                return Builder.CreateMul(L, R, "multmp");
            case BinaryOp::Div:
                return Builder.CreateSDiv(L, R, "divtmp");
        }
    }
    return LogErrorV("Unrecognized operator");
}

ExprAST *BinaryExprAST::GetLHS() {
    return this->LHS.get();
}

ExprAST *BinaryExprAST::GetRHS() {
    return this->RHS.get();
}

BinaryOp BinaryExprAST::GetOp() {
    return this->op;
}

void BinaryExprAST::SetLHS(ExprAST *lhs) {
    this->LHS.release();
    this->LHS.reset(lhs);
}

void BinaryExprAST::SetRHS(ExprAST *rhs) {
    this->RHS.release();
    this->RHS.reset(rhs);
}

UnaryExprAST::UnaryExprAST(UnaryOp op, std::unique_ptr<ExprAST> value)
        : op(op), value(std::move(value)) {}

std::string UnaryExprAST::print() {
    std::string oper;
    switch (this->op) {
        case UnaryOp::Pos:
            oper = "+";
            break;
        case UnaryOp::Neg:
            oper = "-";
            break;
        case UnaryOp::Not:
            oper = "!";
            break;
    }
    return oper + this->value->print();
}

llvm::Value *UnaryExprAST::generate() {
    llvm::Value *v = this->value->generate();
    if (v->getType()->isFloatTy()) return Builder.CreateFNeg(v);
    else return Builder.CreateNeg(v);
}

UnaryOp UnaryExprAST::GetOp() {
    return this->op;
}

ExprAST *UnaryExprAST::GetExpr() {
    return this->value.get();
}

CallExprAST::CallExprAST(std::string callee, std::vector<std::unique_ptr<ExprAST>> args)
        : callee(std::move(callee)), args(std::move(args)) {}

std::string CallExprAST::print() {
    std::stringstream ss;
    ss << this->callee << "(";
    for (unsigned int i = 0; i < this->args.size() - 1; i++) {
        ss << this->args[i]->print() << ", ";
    }
    if (!this->args.empty()) {
        ss << this->args[this->args.size() - 1]->print();
    }
    ss << ")";
    return ss.str();
}

llvm::Value *CallExprAST::generate() {
    llvm::Function *calleeF = TheModule->getFunction(this->callee);
    if (calleeF == nullptr)
        return LogErrorV("Unknown function referenced");

    // If argument mismatch error.
    if (calleeF->arg_size() != this->args.size())
        return LogErrorV("Incorrect # arguments passed");

    std::vector<llvm::Value *> argsV;
    for (unsigned i = 0; i < this->args.size(); ++i) {
        argsV.push_back(this->args[i]->generate());
        if (argsV.back() == nullptr)
            return nullptr;
    }
    if (calleeF->getReturnType()->isVoidTy()) {
        return Builder.CreateCall(calleeF, argsV);
    } else {
        return Builder.CreateCall(calleeF, argsV, "calltmp");
    }
}

std::string CallExprAST::GetCallee() {
    return this->callee;
}

const std::vector<std::unique_ptr<ExprAST>> &CallExprAST::GetArgs() {
    return this->args;
}

VariableExprAST::VariableExprAST(std::string name) : name(std::move(name)) {}

std::string VariableExprAST::print() {
    return this->name;
}

llvm::Value *VariableExprAST::generate() {


    if (NamedValues.find(name) != NamedValues.end()) {
        return NamedValues[name];
    } else {
        llvm::AllocaInst *value = FindAlloca(name);
        if (value == nullptr) {
            return LogErrorV("Undefined variable");
        }
        return Builder.CreateLoad(value, name);
    }
}

std::string VariableExprAST::GetName() {
    return this->name;
}

ComparisonExprAST::ComparisonExprAST(ComparisonOp op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
        : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

std::string ComparisonExprAST::print() {
    std::string oper;
    switch (this->op) {
        case ComparisonOp::EQ:
            oper = " == ";
            break;
        case ComparisonOp::LT:
            oper = " < ";
            break;
        case ComparisonOp::LE:
            oper = " <= ";
            break;
        case ComparisonOp::GT:
            oper = " > ";
            break;
        case ComparisonOp::GE:
            oper = " >= ";
            break;
        case ComparisonOp::NE:
            oper = " != ";
            break;
    }

    return this->LHS->print() + oper + this->RHS->print();
}

llvm::Value *ComparisonExprAST::generate() {
    llvm::Value *LHSV = this->LHS->generate();
    auto type = LHSV->getType();
    llvm::Value *RHSV = this->RHS->generate();
    llvm::Type *lhsType = LHSV->getType();
    llvm::Type *rhsType = RHSV->getType();
    if (lhsType->isFloatingPointTy() || rhsType->isFloatingPointTy()) {
        switch (this->op) {
            case ComparisonOp::LT:
                return Builder.CreateFCmpOLT(LHSV, RHSV, "ifcond");
            case ComparisonOp::LE:
                return Builder.CreateFCmpOLE(LHSV, RHSV, "ifcond");
            case ComparisonOp::GT:
                return Builder.CreateFCmpOGT(LHSV, RHSV, "ifcond");
            case ComparisonOp::GE:
                return Builder.CreateFCmpOGE(LHSV, RHSV, "ifcond");
            case ComparisonOp::EQ:
                return Builder.CreateFCmpOEQ(LHSV, RHSV, "ifcond");
            case ComparisonOp::NE:
                return Builder.CreateFCmpONE(LHSV, RHSV, "ifcond");
        }
    } else if (lhsType->isIntegerTy() && rhsType->isIntegerTy()) {
        switch (this->op) {
            case ComparisonOp::LT:
                return Builder.CreateICmpSLT(LHSV, RHSV, "ifcond");
            case ComparisonOp::LE:
                return Builder.CreateICmpSLE(LHSV, RHSV, "ifcond");
            case ComparisonOp::GT:
                return Builder.CreateICmpSGT(LHSV, RHSV, "ifcond");
            case ComparisonOp::GE:
                return Builder.CreateICmpSGE(LHSV, RHSV, "ifcond");
            case ComparisonOp::EQ:
                return Builder.CreateICmpEQ(LHSV, RHSV, "ifcond");
            case ComparisonOp::NE:
                return Builder.CreateICmpNE(LHSV, RHSV, "ifcond");
        }

    }
    return LogErrorV("Unrecognized operator");
}

ComparisonOp ComparisonExprAST::GetOp() {
    return this->op;
}

ExprAST *ComparisonExprAST::GetLHS() {
    return this->LHS.get();
}

ExprAST *ComparisonExprAST::GetRHS() {
    return this->RHS.get();
}

void ComparisonExprAST::SetLHS(ExprAST *lhs) {
    this->LHS.release();
    this->LHS.reset(lhs);
}

void ComparisonExprAST::SetRHS(ExprAST *rhs) {
    this->RHS.release();
    this->RHS.reset(rhs);
}

BoolOpExprAST::BoolOpExprAST(BoolOp op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
        : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

std::string BoolOpExprAST::print() {
    std::string oper;
    switch (this->op) {
        case BoolOp::AND:
            oper = " && ";
            break;
        case BoolOp::OR:
            oper = " || ";
            break;
    }

    return this->LHS->print() + oper + this->RHS->print();
}

llvm::Value *BoolOpExprAST::generate() {
    llvm::Value *lhs = this->LHS->generate();
    llvm::Value *rhs = this->RHS->generate();
    switch (this->op) {
        case BoolOp::AND:
            return Builder.CreateAnd(lhs, rhs);
        case BoolOp::OR:
            return Builder.CreateOr(lhs, rhs);
    }
    return LogErrorV("Not yet implemented");
}

BoolOp BoolOpExprAST::GetOp() {
    return this->op;
}

ExprAST *BoolOpExprAST::GetLHS() {
    return this->LHS.get();
}

ExprAST *BoolOpExprAST::GetRHS() {
    return this->RHS.get();
}

LiteralBoolExprAST::LiteralBoolExprAST(bool value)
        : value(value) {}

std::string LiteralBoolExprAST::print() {
    return this->value ? "true" : "false";
}

llvm::Value *LiteralBoolExprAST::generate() {
    return LogErrorV("Not yet implemented");
}

bool LiteralBoolExprAST::GetValue() {
    return this->value;
}

DoubleExprAST::DoubleExprAST(double value)
        : value(value) {}

std::string DoubleExprAST::print() {
    return std::to_string(this->value);
}

llvm::Value *DoubleExprAST::generate() {
    return llvm::ConstantFP::get(TheContext, llvm::APFloat(this->value));
}

CastingExprAST::CastingExprAST(std::unique_ptr<ExprAST> value, Type type)
        : value(std::move(value)), type(type) {}

std::string CastingExprAST::print() {
    return ExprAST::print();
}

llvm::Value *CastingExprAST::generate() {
    // TODO Depending on the args, SIToFP has to change into other values
    llvm::Value *v = this->value->generate();
    return Builder.CreateCast(llvm::Instruction::SIToFP, v, GetFromType(this->type));
}

ExprAST *CastingExprAST::GetValue() {
    return this->value.get();
}

Type CastingExprAST::GetType() {
    return this->type;
}
