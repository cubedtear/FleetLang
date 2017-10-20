// Created by aritz.

#pragma once

#include <string>
#include <memory>
#include <array>
#include "ExprAST.h"
#include "../Helpers.h"
#include "Type.h"

// @formatter:off
//                                                      BYTE          CHAR          SHORT         INT           LONG          FLOAT         DOUBLE        BOOL        VOID
constexpr std::array<std::array<Type, 9>, 9> TypeCoercions{{{{Type::Byte,   Type::Byte,   Type::Short,  Type::Int,    Type::Long,   Type::Float,  Type::Double, Type::Void, Type::Void}},   // BYTE
                                                            {{Type::Byte,   Type::Char,   Type::Short,  Type::Int,    Type::Long,   Type::Float,  Type::Double, Type::Void, Type::Void}},   // CHAR
                                                            {{Type::Short,  Type::Short,  Type::Short,  Type::Int,    Type::Long,   Type::Float,  Type::Double, Type::Void, Type::Void}},   // SHORT
                                                            {{Type::Int,    Type::Int,    Type::Int,    Type::Int,    Type::Long,   Type::Float,  Type::Double, Type::Void, Type::Void}},   // INT
                                                            {{Type::Long,   Type::Long,   Type::Long,   Type::Long,   Type::Long,   Type::Double, Type::Double, Type::Void, Type::Void}},   // LONG
                                                            {{Type::Float,  Type::Float,  Type::Float,  Type::Float,  Type::Double, Type::Float,  Type::Double, Type::Void, Type::Void}},   // FLOAT
                                                            {{Type::Double, Type::Double, Type::Double, Type::Double, Type::Double, Type::Double, Type::Double, Type::Void, Type::Void}},   // DOUBLE
                                                            {{Type::Void,   Type::Void,   Type::Void,   Type::Void,   Type::Void,   Type::Void,   Type::Void,   Type::Bool, Type::Void}},   // BOOL
                                                            {{Type::Void,   Type::Void,   Type::Void,   Type::Void,   Type::Void,   Type::Void,   Type::Void,   Type::Void, Type::Void}}}}; // VOID

constexpr std::array<int, 9> TypeSizes{/*Byte*/   1,
                                       /*Char*/   1,
                                       /*Short*/  2,
                                       /*Int*/    4,
                                       /*Long*/   8,
                                       /*Float*/  4,
                                       /*Double*/ 8,
                                       /*Bool*/   1,
                                       /*Void*/   0};

// @formatter:on

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

    VarDeclStmtAST *GetDecl();

    std::string GetLVar();

    ExprAST *GetValue();
};

class IfStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> tru;
    std::unique_ptr<StmtAST> fals;
public:
    IfStmtAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<StmtAST> tru, std::unique_ptr<StmtAST> fals);

    std::string print() override;

    void generate() override;

    ExprAST *GetCond();

    StmtAST *GetTrue();

    StmtAST *GetFalse();
};

class WhileStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<StmtAST> body;

public:
    WhileStmtAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<StmtAST> body);

    std::string print() override;

    void generate() override;

    ExprAST *GetCond();

    StmtAST *GetBody();
};

class ReturnStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> value;
public:
    explicit ReturnStmtAST(std::unique_ptr<ExprAST> value);

    std::string print() override;

    void generate() override;

    ExprAST *GetExpr();
};

class ExprStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> expr;

public:
    explicit ExprStmtAST(std::unique_ptr<ExprAST> expr);

    std::string print() override;

    void generate() override;

    ExprAST *GetExpr();
};

class BlockStmtAST : public StmtAST {
    std::vector<std::unique_ptr<StmtAST>> stmts;
public:
    explicit BlockStmtAST(std::vector<std::unique_ptr<StmtAST>> stmts);

    std::string print() override;

    void generate() override;

    const std::vector<std::unique_ptr<StmtAST>> &GetStmts();
};

class VarDeclStmtAST : public StmtAST {
    std::string name;
    Type type;
public:
    VarDeclStmtAST(std::string name, Type type);

    std::string print() override;

    void generate() override;

    friend class AssignStmtAST;

    std::string GetName();

    Type GetType();
};