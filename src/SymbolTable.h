// Created by aritz.

#pragma once


#include <memory>
#include <vector>
#include <map>
#include "AST/StmtAST.h"
#include "AST/Function.h"

class SymbolTable {

    SymbolTable *parent;
    std::vector<std::unique_ptr<SymbolTable>> children;

    std::map<std::string, VarDeclStmtAST *> variables;
    std::map<std::string, FunctionDeclaration *> functions;

public:
    SymbolTable();

    explicit SymbolTable(SymbolTable *parent);

    ~SymbolTable() = default;

    SymbolTable *addChild(SymbolTable *child);

    void addVariable(VarDeclStmtAST *v);

    void addFunction(FunctionDeclaration *f);

    VarDeclStmtAST *FindVariable(std::string name);

    FunctionDeclaration *FindFunction(std::string name);

    VarDeclStmtAST *FindVariableInCurrentScope(std::string name);

    FunctionDeclaration *FindFunctionInCurrentScope(std::string name);

    SymbolTable *GetParent();


};


