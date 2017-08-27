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
    std::vector<std::pair<std::string, Type>> args;
public:
    FunctionDeclaration(Type ret, std::string name, std::vector<std::pair<std::string, Type>> args);
    virtual llvm::Function *generate();
    virtual std::string print();
};

class Function : public FunctionDeclaration {
    std::vector<std::unique_ptr<StmtAST>> stmts;

public:
    explicit Function(Type ret, std::string name, std::vector<std::pair<std::string, Type>> args, std::vector<std::unique_ptr<StmtAST>> stmts);

    std::string print() override;
    llvm::Function *generate() override;
};


