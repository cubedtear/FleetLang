// Created by aritz.

#include "SymbolTable.h"

SymbolTable::SymbolTable(SymbolTable *parent)
        : parent(parent) {
    if (parent != nullptr) {
        parent->addChild(this);
    }
}

SymbolTable *SymbolTable::addChild(SymbolTable *child) {
    this->children.push_back(std::move(std::unique_ptr<SymbolTable>(child)));
    return child;
}

void SymbolTable::addVariable(VarDeclStmtAST *v) {
    variables.insert(std::make_pair(v->GetName(), v));
}

void SymbolTable::addFunction(FunctionDeclaration *f) {
    functions.insert(std::make_pair(f->GetName(), f));
}

VarDeclStmtAST *SymbolTable::FindVariable(std::string name) {
    auto v = this->variables.find(name);
    if (v != this->variables.end()) return v->second;
    if (this->parent != nullptr) {
        auto var = this->parent->FindVariable(name);
        if (var != nullptr) return var;
    }
    return nullptr;
}

FunctionDeclaration *SymbolTable::FindFunction(std::string name) {
    auto f = this->functions.find(name);
    if (f != this->functions.end()) return f->second;
    if (this->parent != nullptr) {
        auto var = this->parent->FindFunction(name);
        if (var != nullptr) return var;
    }
    return nullptr;
}

SymbolTable::SymbolTable()
        : SymbolTable(nullptr) {}

SymbolTable *SymbolTable::GetParent() {
    return this->parent;
}

VarDeclStmtAST *SymbolTable::FindVariableInCurrentScope(std::string name) {
    auto v = this->variables.find(name);
    if (v != this->variables.end()) return v->second;
    return nullptr;
}

FunctionDeclaration *SymbolTable::FindFunctionInCurrentScope(std::string name) {
    auto f = this->functions.find(name);
    if (f != this->functions.end()) return f->second;
    return nullptr;
}


