// Created by aritz.

#include <FleetLangLexer.h>
#include "NewVisitor.h"
#include "AST/StmtAST.h"
#include "AST/Function.h"
#include "AST/Program.h"
#include "Helpers.h"

antlrcpp::Any NewVisitor::visitProgram(FleetLangParser::ProgramContext *ctx) {
    std::vector<std::unique_ptr<FunctionDeclaration>> functions;
    for (auto f : ctx->functions) { ;
        functions.push_back(std::unique_ptr<FunctionDeclaration>(this->visit(f).as<FunctionDeclaration *>()));
    }
    return new Program(std::move(functions));
}

antlrcpp::Any NewVisitor::visitAssignStmt(FleetLangParser::AssignStmtContext *ctx) {
    VarDeclStmtAST *decl;

    if (ctx->type() == nullptr) {
        decl = nullptr;
    } else {
        decl = new VarDeclStmtAST(ctx->left->getText(), this->visit(ctx->type()).as<Type>());
    }

    return (StmtAST *) new AssignStmtAST(std::unique_ptr<VarDeclStmtAST>(decl), ctx->left->getText(),
                                         std::unique_ptr<ExprAST>(this->visit(ctx->right).as<ExprAST *>()));
}

antlrcpp::Any NewVisitor::visitIfStmt(FleetLangParser::IfStmtContext *ctx) {
    ExprAST *cond = this->visit(ctx->cond).as<ExprAST *>();
    StmtAST *tru = ctx->whenTrue != nullptr ? this->visit(ctx->whenTrue).as<StmtAST *>() : nullptr;
    StmtAST *fals = ctx->whenFalse != nullptr ? this->visit(ctx->whenFalse).as<StmtAST *>() : nullptr;

    return (StmtAST *) new IfStmtAST(std::unique_ptr<ExprAST>(cond), std::unique_ptr<StmtAST>(tru),
                                     std::unique_ptr<StmtAST>(fals));
}

antlrcpp::Any NewVisitor::visitWhileStmt(FleetLangParser::WhileStmtContext *ctx) {
    ExprAST *cond = this->visit(ctx->cond).as<ExprAST *>();
    StmtAST *body = ctx->whenTrue != nullptr ? this->visit(ctx->whenTrue).as<StmtAST *>() : nullptr;
    return (StmtAST *) new WhileStmtAST(std::unique_ptr<ExprAST>(cond), std::unique_ptr<StmtAST>(body));
}

antlrcpp::Any NewVisitor::visitReturnStmt(FleetLangParser::ReturnStmtContext *ctx) {
    ExprAST *expr = ctx->expr != nullptr ? this->visit(ctx->expr).as<ExprAST *>() : nullptr;
    return (StmtAST *) new ReturnStmtAST(std::unique_ptr<ExprAST>(expr));
}

antlrcpp::Any NewVisitor::visitBlockStmt(FleetLangParser::BlockStmtContext *ctx) {
    std::vector<std::unique_ptr<StmtAST>> stmts;
    for (auto s : ctx->stmts) {
        if (s != nullptr) {
            stmts.push_back(std::unique_ptr<StmtAST>(this->visit(s).as<StmtAST *>()));
        }
    }
    return (StmtAST *) new BlockStmtAST(std::move(stmts));
}

antlrcpp::Any NewVisitor::visitDeclStmt(FleetLangParser::DeclStmtContext *ctx) {
    return (StmtAST *) new VarDeclStmtAST(ctx->name->getText(), this->visit(ctx->type()).as<Type>());
}

antlrcpp::Any NewVisitor::visitExpressionStmt(FleetLangParser::ExpressionStmtContext *ctx) {
    ExprAST *expr = this->visit(ctx->expr).as<ExprAST *>();
    return (StmtAST *) new ExprStmtAST(std::unique_ptr<ExprAST>(expr));
}

antlrcpp::Any NewVisitor::visitIntExpr(FleetLangParser::IntExprContext *ctx) {
    int value = std::stoi(ctx->getText());
    return (ExprAST *) new IntExprAST(value);
}

antlrcpp::Any NewVisitor::visitFuncExpr(FleetLangParser::FuncExprContext *ctx) {
    std::string callee = ctx->func->getText();
    std::vector<std::unique_ptr<ExprAST>> args;

    for (auto a : ctx->args) {
        args.push_back(std::unique_ptr<ExprAST>(this->visit(a).as<ExprAST *>()));
    }

    return (ExprAST *) new CallExprAST(callee, std::move(args));
}

antlrcpp::Any NewVisitor::visitOpExpr(FleetLangParser::OpExprContext *ctx) {
    ExprAST *lhs = this->visit(ctx->left).as<ExprAST *>();
    ExprAST *rhs = this->visit(ctx->right).as<ExprAST *>();

    BinaryOp op;
    switch (ctx->op->getType()) {
        case FleetLangLexer::ADD:
            op = BinaryOp::Add;
            break;
        case FleetLangLexer::SUBTRACT:
            op = BinaryOp::Sub;
            break;
        case FleetLangLexer::MULTIPLY:
            op = BinaryOp::Mul;
            break;
        case FleetLangLexer::DIVIDE:
            op = BinaryOp::Div;
            break;
        default:
            // TODO ERROR
            ;
    }

    return (ExprAST *) new BinaryExprAST(op, std::unique_ptr<ExprAST>(lhs), std::unique_ptr<ExprAST>(rhs));
}

antlrcpp::Any NewVisitor::visitParenBool(FleetLangParser::ParenBoolContext *ctx) {
    return this->visit(ctx->expr);
}

antlrcpp::Any NewVisitor::visitLiteralBool(FleetLangParser::LiteralBoolContext *ctx) {
    return (ExprAST *) new LiteralBoolExprAST(ctx->atom->getType() == FleetLangLexer::TRUE);
}

antlrcpp::Any NewVisitor::visitComparisonOp(FleetLangParser::ComparisonOpContext *ctx) {
    ExprAST *lhs = this->visit(ctx->left).as<ExprAST *>();
    ExprAST *rhs = this->visit(ctx->right).as<ExprAST *>();

    ComparisonOp op;
    switch (ctx->op->getType()) {
        case FleetLangLexer::LESS_THAN:
            op = ComparisonOp::LT;
            break;
        case FleetLangLexer::LESS_EQUAL:
            op = ComparisonOp::LE;
            break;
        case FleetLangLexer::GREATER_THAN:
            op = ComparisonOp::GT;
            break;
        case FleetLangLexer::GREATER_EQUAL:
            op = ComparisonOp::GE;
            break;
        case FleetLangLexer::EQUALS:
            op = ComparisonOp::EQ;
            break;
        case FleetLangLexer::NOT_EQUALS:
            op = ComparisonOp::NE;
            break;
        default:
            // TODO ERROR
            ;
    }

    return (ExprAST *) new ComparisonExprAST(op, std::unique_ptr<ExprAST>(lhs), std::unique_ptr<ExprAST>(rhs));
}

antlrcpp::Any NewVisitor::visitStringExpr(FleetLangParser::StringExprContext *ctx) {
    return (ExprAST *) new StringExprAST(ctx->atom->getText());
}

antlrcpp::Any NewVisitor::visitFloatExpr(FleetLangParser::FloatExprContext *ctx) {
    float value = std::stof(ctx->atom->getText());
    return (ExprAST *) new FloatExprAST(value);
}

antlrcpp::Any NewVisitor::visitSignedExpression(FleetLangParser::SignedExpressionContext *ctx) {
    UnaryOp op = ctx->sign->getType() == FleetLangLexer::ADD ? UnaryOp::Pos : UnaryOp::Neg;
    ExprAST *expr = this->visit(ctx->expr).as<ExprAST *>();
    return (ExprAST *) new UnaryExprAST(op, std::unique_ptr<ExprAST>(expr));
}

antlrcpp::Any NewVisitor::visitBooleanOp(FleetLangParser::BooleanOpContext *ctx) {
    BoolOp op;
    switch (ctx->op->getType()) {
        case FleetLangLexer::BOOL_AND:
            op = BoolOp::AND;
            break;
        case FleetLangLexer::BOOL_OR:
            op = BoolOp::OR;
            break;
        default:
            // TODO ERROR
            ;
    }

    ExprAST *lhs = this->visit(ctx->left).as<ExprAST *>();
    ExprAST *rhs = this->visit(ctx->right).as<ExprAST *>();

    return (ExprAST *) new BoolOpExprAST(op, std::unique_ptr<ExprAST>(lhs), std::unique_ptr<ExprAST>(rhs));
}

antlrcpp::Any NewVisitor::visitTupleExpr(FleetLangParser::TupleExprContext *ctx) {
    return FleetLangBaseVisitor::visitTupleExpr(ctx);
}

antlrcpp::Any NewVisitor::visitIdExpr(FleetLangParser::IdExprContext *ctx) {
    return (ExprAST *) new VariableExprAST(ctx->atom->getText());
}

antlrcpp::Any NewVisitor::visitFunction(FleetLangParser::FunctionContext *ctx) {

    std::vector<std::unique_ptr<VarDeclStmtAST>> args;

    for (unsigned int i = 0; i < ctx->argNames.size(); i++) {
        Type argType = this->visit(ctx->args[i]).as<Type>();
        args.push_back(std::make_unique<VarDeclStmtAST>(ctx->argNames[i]->getText(), argType));
    }

    Type type;
    if (ctx->retType != nullptr) type = this->visit(ctx->retType).as<Type>();
    else type = this->visit(ctx->voidTy).as<Type>();

    if (ctx->stmts.empty()) {
        return new FunctionDeclaration(type, ctx->name->getText(), std::move(args));
    }

    std::vector<std::unique_ptr<StmtAST>> stmts;

    for (auto s : ctx->stmts) {
        StmtAST *p = this->visit(s).as<StmtAST *>();
        if (p != nullptr) stmts.push_back(std::unique_ptr<StmtAST>(p));
    }
    return (FunctionDeclaration *) new Function(type, ctx->name->getText(), std::move(args), std::move(stmts));
}

antlrcpp::Any NewVisitor::visitEmptyStmt(FleetLangParser::EmptyStmtContext *ctx) {
    return (StmtAST *) nullptr;
}

antlrcpp::Any NewVisitor::visitPrimType(FleetLangParser::PrimTypeContext *ctx) {
    return this->visit(ctx->name);
}

antlrcpp::Any NewVisitor::visitIntType(FleetLangParser::IntTypeContext *ctx) {
    return Type::Int;
}

antlrcpp::Any NewVisitor::visitFloatType(FleetLangParser::FloatTypeContext *ctx) {
    return Type::Float;
}

antlrcpp::Any NewVisitor::visitBoolType(FleetLangParser::BoolTypeContext *ctx) {
    return Type::Bool;
}

antlrcpp::Any NewVisitor::visitVoidType(FleetLangParser::VoidTypeContext *ctx) {
    return Type::Void;
}

antlrcpp::Any NewVisitor::visitCharType(FleetLangParser::CharTypeContext *ctx) {
    return Type::Char;
}

antlrcpp::Any NewVisitor::visitByteType(FleetLangParser::ByteTypeContext *ctx) {
    return Type::Byte;
}

antlrcpp::Any NewVisitor::visitShortType(FleetLangParser::ShortTypeContext *ctx) {
    return Type::Short;
}

antlrcpp::Any NewVisitor::visitLongType(FleetLangParser::LongTypeContext *ctx) {
    return Type::Long;
}

antlrcpp::Any NewVisitor::visitDoubleType(FleetLangParser::DoubleTypeContext *ctx) {
    return Type::Double;
}

antlrcpp::Any NewVisitor::visitDoubleExpr(FleetLangParser::DoubleExprContext *ctx) {
    double value = std::stod(ctx->atom->getText());
    return (ExprAST *) new DoubleExprAST(value);
}


