#pragma once


#include <FleetLangParser.h>
#include <FleetLangVisitor.h>
#include <FleetLangBaseVisitor.h>
#include "AST/Types.h"

class InMemoryVisitor : public FleetLangBaseVisitor {
public:

    virtual antlrcpp::Any visitProgram(FleetLangParser::ProgramContext *ctx) override;
    virtual antlrcpp::Any visitFunction(FleetLangParser::FunctionContext *ctx) override;

    virtual antlrcpp::Any visitArrayType(FleetLangParser::ArrayTypeContext *ctx) override;
    virtual antlrcpp::Any visitRefType(FleetLangParser::RefTypeContext *ctx) override;
    virtual antlrcpp::Any visitTupleType(FleetLangParser::TupleTypeContext *ctx) override;
    virtual antlrcpp::Any visitPrimType(FleetLangParser::PrimTypeContext *ctx) override;
    virtual antlrcpp::Any visitIntType(FleetLangParser::IntTypeContext *ctx) override;
    virtual antlrcpp::Any visitFloatType(FleetLangParser::FloatTypeContext *ctx) override;
    virtual antlrcpp::Any visitBoolType(FleetLangParser::BoolTypeContext *ctx) override;
    virtual antlrcpp::Any visitCharType(FleetLangParser::CharTypeContext *ctx) override;

    virtual antlrcpp::Any visitAssignStmt(FleetLangParser::AssignStmtContext *ctx) override;
    virtual antlrcpp::Any visitIfStmt(FleetLangParser::IfStmtContext *ctx) override;
    virtual antlrcpp::Any visitWhileStmt(FleetLangParser::WhileStmtContext *ctx) override;
    virtual antlrcpp::Any visitReturnStmt(FleetLangParser::ReturnStmtContext *ctx) override;
    virtual antlrcpp::Any visitEmptyStmt(FleetLangParser::EmptyStmtContext *ctx) override;
    virtual antlrcpp::Any visitBlockStmt(FleetLangParser::BlockStmtContext *ctx) override;
    virtual antlrcpp::Any visitDeclStmt(FleetLangParser::DeclStmtContext *ctx) override;
    virtual antlrcpp::Any visitExpressionStmt(FleetLangParser::ExpressionStmtContext *ctx) override;

    virtual antlrcpp::Any visitStringExpr(FleetLangParser::StringExprContext *ctx) override;
    virtual antlrcpp::Any visitFloatExpr(FleetLangParser::FloatExprContext *ctx) override;
    virtual antlrcpp::Any visitIntExpr(FleetLangParser::IntExprContext *ctx) override;
    virtual antlrcpp::Any visitFuncExpr(FleetLangParser::FuncExprContext *ctx) override;
    virtual antlrcpp::Any visitOpExpr(FleetLangParser::OpExprContext *ctx) override;
    virtual antlrcpp::Any visitParenExpr(FleetLangParser::ParenExprContext *ctx) override;
    virtual antlrcpp::Any visitTupleExpr(FleetLangParser::TupleExprContext *ctx) override;
    virtual antlrcpp::Any visitSignedExpression(FleetLangParser::SignedExpressionContext *ctx) override;
    virtual antlrcpp::Any visitIdExpr(FleetLangParser::IdExprContext *ctx) override;

    virtual antlrcpp::Any visitComparisonOp(FleetLangParser::ComparisonOpContext *ctx) override;
    virtual antlrcpp::Any visitBooleanOp(FleetLangParser::BooleanOpContext *ctx) override;
    virtual antlrcpp::Any visitParenBool(FleetLangParser::ParenBoolContext *ctx) override;
    virtual antlrcpp::Any visitLiteralBool(FleetLangParser::LiteralBoolContext *ctx) override;

};



