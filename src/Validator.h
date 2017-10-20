// Created by aritz.

#pragma once


#include "AST/Program.h"
#include "SymbolTable.h"

class Validator {
    SymbolTable *table;
    FunctionDeclaration *currentFunction = nullptr;

public:
    Validator();

    ~Validator();

    bool check(Program *p);

private:

    void pushScope();

    void popScope();

    bool check(FunctionDeclaration *f);

    bool check(Function *f);

    bool check(StmtAST *s);

    bool check(AssignStmtAST *s);

    bool check(IfStmtAST *s);

    bool check(WhileStmtAST *s);

    bool check(ReturnStmtAST *s);

    bool check(ExprStmtAST *s);

    bool check(BlockStmtAST *s);

    bool check(VarDeclStmtAST *s);


    std::pair<bool, Type> check(ExprAST *e);

    std::pair<bool, Type> check(StringExprAST *e);

    std::pair<bool, Type> check(FloatExprAST *e);

    std::pair<bool, Type> check(DoubleExprAST *e);

    std::pair<bool, Type> check(IntExprAST *e);

    std::pair<bool, Type> check(LiteralBoolExprAST *e);

    std::pair<bool, Type> check(BinaryExprAST *e);

    std::pair<bool, Type> check(UnaryExprAST *e);

    std::pair<bool, Type> check(CallExprAST *e);

    std::pair<bool, Type> check(VariableExprAST *e);

    std::pair<bool, Type> check(ComparisonExprAST *e);

    std::pair<bool, Type> check(BoolOpExprAST *e);

    std::pair<bool, Type> check(CastingExprAST *e);

    Type CanCoerce(Type a, Type b);

    Type SmallestType(Type a, Type b);
};


