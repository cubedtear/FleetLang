// Created by aritz.

#pragma once


#include <memory>
#include "StmtAST.h"
#include "CodegenHelper.h"

class Function;

class FunctionDeclaration {
protected:
    Type ret;
    std::string name;
    std::vector<std::unique_ptr<VarDeclStmtAST>> args;
public:
    virtual ~FunctionDeclaration() = default;
    FunctionDeclaration(Type ret, std::string name, std::vector<std::unique_ptr<VarDeclStmtAST>> args);
    virtual llvm::Function *generate();
    virtual std::string print();

    std::string GetName();
    Type GetReturnType();
    const std::vector<std::unique_ptr<VarDeclStmtAST>> &GetArgs();

    bool operator==(const FunctionDeclaration &other) const;
    bool operator!=(const FunctionDeclaration &other) const;
};

class Function : public FunctionDeclaration {
    std::vector<std::unique_ptr<StmtAST>> stmts;

public:
    ~Function() override = default;
    explicit Function(Type ret, std::string name, std::vector<std::unique_ptr<VarDeclStmtAST>> args, std::vector<std::unique_ptr<StmtAST>> stmts);

    std::string print() override;
    llvm::Function *generate() override;

    const std::vector<std::unique_ptr<StmtAST>> &GetStmts();
};


