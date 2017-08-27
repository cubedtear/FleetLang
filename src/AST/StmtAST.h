// Created by aritz.

#pragma once

#include <string>
#include <memory>
#include "ExprAST.h"

enum class Type {
    Int, Float, Bool, Void
};


class StmtAST {
public:
    virtual ~StmtAST() = default;
    virtual std::string print() { throw "Not implemented"; }
    virtual void generate() = 0;
};

class VarDeclStmtAST;

class AssignStmtAST : public StmtAST {
    std::unique_ptr<VarDeclStmtAST> decl;
    std::string lvar;
    std::unique_ptr<ExprAST> value;
public:
    AssignStmtAST(std::unique_ptr<VarDeclStmtAST> decl, std::string lvar, std::unique_ptr<ExprAST> value);
    std::string print() override;
    void generate() override;
};

class IfStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> tru;
    std::unique_ptr<StmtAST> fals;
public:
    IfStmtAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<StmtAST> tru, std::unique_ptr<StmtAST> fals);
    std::string print() override;
    void generate() override;
};

class WhileStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> body;

public:
    WhileStmtAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<StmtAST> body);
    std::string print() override;
    void generate() override;
};

class ReturnStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> value;
public:
    explicit ReturnStmtAST(std::unique_ptr<ExprAST> value);
    std::string print() override;
    void generate() override;
};

class ExprStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> expr;

public:
    explicit ExprStmtAST(std::unique_ptr<ExprAST> expr);
    std::string print() override;
    void generate() override;
};

class BlockStmtAST : public StmtAST {
    std::vector<std::unique_ptr<StmtAST>> stmts;
public:
    explicit BlockStmtAST(std::vector<std::unique_ptr<StmtAST>> stmts);
    std::string print() override;
    void generate() override;
};

class VarDeclStmtAST : public StmtAST {
    std::string name;
    Type type;
public:
    VarDeclStmtAST(std::string name, Type type);

    std::string print() override;

    void generate() override;

    friend class AssignStmtAST;
};