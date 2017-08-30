// Created by aritz.

#pragma once


#include <FleetLangBaseVisitor.h>

class NewVisitor : public FleetLangBaseVisitor {
public:
    antlrcpp::Any visitProgram(FleetLangParser::ProgramContext *ctx) override;

    antlrcpp::Any visitFunction(FleetLangParser::FunctionContext *ctx) override;

    antlrcpp::Any visitAssignStmt(FleetLangParser::AssignStmtContext *ctx) override;

    antlrcpp::Any visitIfStmt(FleetLangParser::IfStmtContext *ctx) override;

    antlrcpp::Any visitWhileStmt(FleetLangParser::WhileStmtContext *ctx) override;

    antlrcpp::Any visitReturnStmt(FleetLangParser::ReturnStmtContext *ctx) override;

    antlrcpp::Any visitBlockStmt(FleetLangParser::BlockStmtContext *ctx) override;

    antlrcpp::Any visitDeclStmt(FleetLangParser::DeclStmtContext *ctx) override;

    antlrcpp::Any visitExpressionStmt(FleetLangParser::ExpressionStmtContext *ctx) override;

    antlrcpp::Any visitIntExpr(FleetLangParser::IntExprContext *ctx) override;

    antlrcpp::Any visitFuncExpr(FleetLangParser::FuncExprContext *ctx) override;

    antlrcpp::Any visitOpExpr(FleetLangParser::OpExprContext *ctx) override;

    antlrcpp::Any visitParenBool(FleetLangParser::ParenBoolContext *ctx) override;

    antlrcpp::Any visitLiteralBool(FleetLangParser::LiteralBoolContext *ctx) override;

    antlrcpp::Any visitComparisonOp(FleetLangParser::ComparisonOpContext *ctx) override;

    antlrcpp::Any visitStringExpr(FleetLangParser::StringExprContext *ctx) override;

    antlrcpp::Any visitFloatExpr(FleetLangParser::FloatExprContext *ctx) override;

    antlrcpp::Any visitSignedExpression(FleetLangParser::SignedExpressionContext *ctx) override;

    antlrcpp::Any visitBooleanOp(FleetLangParser::BooleanOpContext *ctx) override;

    antlrcpp::Any visitTupleExpr(FleetLangParser::TupleExprContext *ctx) override;

    antlrcpp::Any visitIdExpr(FleetLangParser::IdExprContext *ctx) override;

    antlrcpp::Any visitEmptyStmt(FleetLangParser::EmptyStmtContext *ctx) override;

    antlrcpp::Any visitPrimType(FleetLangParser::PrimTypeContext *ctx) override;

    antlrcpp::Any visitIntType(FleetLangParser::IntTypeContext *ctx) override;

    antlrcpp::Any visitFloatType(FleetLangParser::FloatTypeContext *ctx) override;

    antlrcpp::Any visitBoolType(FleetLangParser::BoolTypeContext *ctx) override;

    antlrcpp::Any visitVoidType(FleetLangParser::VoidTypeContext *ctx) override;

    antlrcpp::Any visitCharType(FleetLangParser::CharTypeContext *ctx) override;

    antlrcpp::Any visitByteType(FleetLangParser::ByteTypeContext *ctx) override;

    antlrcpp::Any visitShortType(FleetLangParser::ShortTypeContext *ctx) override;

    antlrcpp::Any visitLongType(FleetLangParser::LongTypeContext *ctx) override;

    antlrcpp::Any visitDoubleType(FleetLangParser::DoubleTypeContext *ctx) override;

    antlrcpp::Any visitDoubleExpr(FleetLangParser::DoubleExprContext *ctx) override;
};


