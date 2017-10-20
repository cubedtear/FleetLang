// Created by aritz.

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <llvm/IR/Value.h>
#include "Type.h"

enum class BinaryOp {
    Add,
    Sub,
    Mul,
    Div,
    // TODO Modulus
};

enum class UnaryOp {
    Pos,
    Neg,
    Not,
};

enum class ComparisonOp {
    LT,
    LE,
    GT,
    GE,
    EQ,
    NE,
};

enum class BoolOp {
    AND,
    OR,
};


class ExprAST {
public:
    virtual ~ExprAST() = default;

    virtual std::string print() { return ""; };

    virtual llvm::Value *generate() = 0;
};

class StringExprAST : public ExprAST {
    std::string value;
public:
    explicit StringExprAST(std::string value);

    std::string print() override;

    llvm::Value *generate() override;
};

class FloatExprAST : public ExprAST {
    float value;
public:
    explicit FloatExprAST(float value);

    std::string print() override;

    llvm::Value *generate() override;
};

class DoubleExprAST : public ExprAST {
    double value;
public:
    explicit DoubleExprAST(double value);

    std::string print() override;

    llvm::Value *generate() override;
};

class IntExprAST : public ExprAST {
    int value;
public:
    explicit IntExprAST(int value);

    std::string print() override;

    llvm::Value *generate() override;
};

class BinaryExprAST : public ExprAST {
    std::unique_ptr<ExprAST> LHS, RHS;
    BinaryOp op;
public:
    BinaryExprAST(BinaryOp op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS);

    std::string print() override;

    llvm::Value *generate() override;

    ExprAST *GetLHS();
    ExprAST *GetRHS();
    BinaryOp  GetOp();

    void SetLHS(ExprAST *lhs);
    void SetRHS(ExprAST *rhs);
};

class UnaryExprAST : public ExprAST {
    UnaryOp op;
    std::unique_ptr<ExprAST> value;
public:
    UnaryExprAST(UnaryOp op, std::unique_ptr<ExprAST> value);

    std::string print() override;

    llvm::Value *generate() override;

    UnaryOp GetOp();
    ExprAST *GetExpr();
};

class CallExprAST : public ExprAST {
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;
public:
    CallExprAST(std::string callee, std::vector<std::unique_ptr<ExprAST>> args);

    std::string print() override;

    llvm::Value *generate() override;

    std::string GetCallee();
    const std::vector<std::unique_ptr<ExprAST>> &GetArgs();
};

class VariableExprAST : public ExprAST {
    std::string name;
public:
    explicit VariableExprAST(std::string name);

    std::string print() override;

    llvm::Value *generate() override;

    std::string GetName();
};

class ComparisonExprAST : public ExprAST {
    ComparisonOp op;
    std::unique_ptr<ExprAST> LHS, RHS;
public:
    ComparisonExprAST(ComparisonOp op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS);

    std::string print() override;

    llvm::Value *generate() override;

    ComparisonOp GetOp();
    ExprAST *GetLHS();
    ExprAST *GetRHS();


    void SetLHS(ExprAST *lhs);
    void SetRHS(ExprAST *rhs);
};

class BoolOpExprAST : public ExprAST {
    BoolOp op;
    std::unique_ptr<ExprAST> LHS, RHS;
public:
    BoolOpExprAST(BoolOp op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS);

    std::string print() override;

    llvm::Value *generate() override;

    BoolOp GetOp();
    ExprAST *GetLHS();
    ExprAST *GetRHS();
};

class LiteralBoolExprAST : public ExprAST {
    bool value;
public:
    explicit LiteralBoolExprAST(bool value);

    std::string print() override;

    llvm::Value *generate() override;

    bool GetValue();
};

class CastingExprAST : public ExprAST {
    std::unique_ptr<ExprAST> value;
    Type type;
public:
    CastingExprAST(std::unique_ptr<ExprAST> value, Type type);

    std::string print() override;

    llvm::Value *generate() override;

    ExprAST *GetValue();
    Type GetType();
};

//virtual antlrcpp::Any visitComparisonOp(FleetLangParser::ComparisonOpContext *ctx) override;
//virtual antlrcpp::Any visitBooleanOp(FleetLangParser::BooleanOpContext *ctx) override;
//virtual antlrcpp::Any visitParenBool(FleetLangParser::ParenBoolContext *ctx) override;
//virtual antlrcpp::Any visitLiteralBool(FleetLangParser::LiteralBoolContext *ctx) override;
