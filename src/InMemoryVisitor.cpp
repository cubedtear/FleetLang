#include <FleetLangLexer.h>
#include "InMemoryVisitor.h"
#include "Helpers.h"

Location fromToken(antlr4::Token *t) {
    return Location((int) t->getLine(), (int) t->getCharPositionInLine());
}

Location fromTokens(std::initializer_list<antlr4::Token *> list) {
    if (list.size() == 0) return Location(0, 0);
    Location l = fromToken(*list.begin());
    for (auto token : list) {
        if (token != nullptr) l = l.join(fromToken(token));
    }
    return l;
}

Location fromContext(antlr4::ParserRuleContext *ctx) {
    return fromTokens({ctx->getStart(), ctx->getStop()});
}

antlrcpp::Any InMemoryVisitor::visitProgram(FleetLangParser::ProgramContext *ctx) {
    std::vector<Function *> functions;
    for (auto func : ctx->functions) {
        this->visit(func);
        functions.push_back(func->f);
    }
    ctx->p = new Program(functions);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitFunction(FleetLangParser::FunctionContext *ctx) {
    this->visit(ctx->retType);
    std::vector<VarType *> argTypes;
    std::vector<std::string> argNames;
    for (int i = 0; i < ctx->args.size(); i++) {
        this->visit(ctx->args[i]);
        argTypes.push_back(ctx->args[i]->t);
        argNames.push_back(ctx->argNames[i]->getText());
    }
    std::vector<Statement *> stmts;

    for (auto stmt : ctx->stmts) {
        this->visit(stmt);
        if (stmt->stmt != nullptr) stmts.push_back(stmt->stmt);
    }

    ctx->f = new Function(FunctionDeclaration(ctx->retType->t, ctx->name->getText(), argTypes), argNames, stmts, fromContext(ctx));
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitAssignStmt(FleetLangParser::AssignStmtContext *ctx) {
    this->visit(ctx->right);
    if (ctx->varType != nullptr) {
        this->visit(ctx->varType);
        ctx->stmt = new AssignmentStatement(ctx->left->getText(), ctx->right->e, fromContext(ctx), new VariableDeclaration(ctx->varType->t, ctx->left->getText()));
    } else ctx->stmt = new AssignmentStatement(ctx->left->getText(), ctx->right->e, fromContext(ctx));
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitIfStmt(FleetLangParser::IfStmtContext *ctx) {
    this->visit(ctx->cond);
    this->visit(ctx->whenTrue);
    if (ctx->whenFalse != nullptr) this->visit(ctx->whenFalse);

    ctx->stmt = new IfStatement(dynamic_cast<BooleanExpression *>(ctx->cond->e), ctx->whenTrue->stmt, ctx->whenFalse != nullptr ? ctx->whenFalse->stmt : nullptr, fromContext(ctx));
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitWhileStmt(FleetLangParser::WhileStmtContext *ctx) {
    this->visit(ctx->cond);
    this->visit(ctx->whenTrue);
    ctx->stmt = new WhileStatement(dynamic_cast<BooleanExpression *>(ctx->cond->e), ctx->whenTrue->stmt, fromContext(ctx));
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitReturnStmt(FleetLangParser::ReturnStmtContext *ctx) {
    if (ctx->expr != nullptr) {
        this->visit(ctx->expr);
        ctx->stmt = new ReturnStatement(ctx->expr->e, fromContext(ctx));
    } else {
        this->visit(ctx->expr);
        ctx->stmt = new ReturnStatement(dynamic_cast<BooleanExpression *>(ctx->expr->e), fromContext(ctx));
    }
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitEmptyStmt(FleetLangParser::EmptyStmtContext *ctx) {
    ctx->stmt = nullptr;
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitBlockStmt(FleetLangParser::BlockStmtContext *ctx) {
    std::vector<Statement *> stmts;
    for (auto stmt : ctx->stmts) {
        this->visit(stmt);
        if (stmt->stmt != nullptr) stmts.push_back(stmt->stmt);
    }
    ctx->stmt = new BlockStatement(stmts, fromContext(ctx));
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitDeclStmt(FleetLangParser::DeclStmtContext *ctx) {
    // TODO
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitFuncExpr(FleetLangParser::FuncExprContext *ctx) {
    std::vector<Expression *> args;
    for (auto arg : ctx->args) {
        this->visit(arg);
        args.push_back(arg->e);
    }
    ctx->e = new CallExpression(ctx->func->getText(), args);
    return nullptr;
}

ExpressionOperator expressionOperatorFromToken(antlr4::Token *t) {
    switch (t->getType()) {
        case FleetLangLexer::ADD:
            return ExpressionOperator::Add;
        case FleetLangLexer::SUBTRACT:
            return ExpressionOperator::Subtract;
        case FleetLangLexer::MULTIPLY:
            return ExpressionOperator::Multiply;
        case FleetLangLexer::DIVIDE:
            return ExpressionOperator::Divide;
    }
    ERROR("Unknown operator '" + t->getText() + "'!");
}

antlrcpp::Any InMemoryVisitor::visitOpExpr(FleetLangParser::OpExprContext *ctx) {
    this->visit(ctx->left);
    this->visit(ctx->right);
    ExpressionOperator op;

    ctx->e = new OperationExpression(ctx->left->e, expressionOperatorFromToken(ctx->op), ctx->right->e);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitParenExpr(FleetLangParser::ParenExprContext *ctx) {
    this->visit(ctx->expr);
    ctx->e = ctx->expr->e;
    return nullptr;
}

ComparisonOperator comparisonOperatorFromToken(antlr4::Token *t) {
    switch (t->getType()) {
        case FleetLangLexer::LESS_THAN:
            return ComparisonOperator::LT;
        case FleetLangLexer::LESS_EQUAL:
            return ComparisonOperator::LE;
        case FleetLangLexer::GREATER_THAN:
            return ComparisonOperator::GT;
        case FleetLangLexer::GREATER_EQUAL:
            return ComparisonOperator::GE;
        case FleetLangLexer::EQUALS:
            return ComparisonOperator::EQ;
        case FleetLangLexer::NOT_EQUALS:
            return ComparisonOperator::NE;
    }
    ERROR("Comparison operator not recognized '" + t->getText() + "'!");
}

antlrcpp::Any InMemoryVisitor::visitComparisonOp(FleetLangParser::ComparisonOpContext *ctx) {
    this->visit(ctx->left);
    this->visit(ctx->right);
    ctx->e = new ComparisonExpression(ctx->left->e, comparisonOperatorFromToken(ctx->op), ctx->right->e);
    return nullptr;
}

BoolOperator boolOperatorFromToken(antlr4::Token *t) {
    switch (t->getType()) {
        case FleetLangLexer::BOOL_AND:
            return BoolOperator::And;
        case FleetLangLexer::BOOL_OR:
            return BoolOperator::Or;
    }
    ERROR("Boolean operator not recognized '" + t->getText() + "'!");
}

antlrcpp::Any InMemoryVisitor::visitBooleanOp(FleetLangParser::BooleanOpContext *ctx) {
    this->visit(ctx->left);
    this->visit(ctx->right);

    ctx->e = new BoolOpExpression(dynamic_cast<BooleanExpression *>(ctx->left->e), boolOperatorFromToken(ctx->op), dynamic_cast<BooleanExpression *>(ctx->right->e));
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitParenBool(FleetLangParser::ParenBoolContext *ctx) {
    this->visit(ctx->expr);
    ctx->e = ctx->expr->e;
    return FleetLangBaseVisitor::visitParenBool(ctx);
}

antlrcpp::Any InMemoryVisitor::visitLiteralBool(FleetLangParser::LiteralBoolContext *ctx) {
    ctx->e = new LiteralBoolExpression(ctx->atom->getType() == FleetLangLexer::TRUE);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitArrayType(FleetLangParser::ArrayTypeContext *ctx) {
    this->visit(ctx->type());
    ctx->t = new ArrayType(ctx->type()->t, (int) ctx->dim.size());
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitRefType(FleetLangParser::RefTypeContext *ctx) {
    ctx->t = new RefType(ctx->name->getText());
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitTupleType(FleetLangParser::TupleTypeContext *ctx) {
    std::vector<VarType *> types;
    for (auto type : ctx->type()) {
        this->visit(type);
        types.push_back(type->t);
    }
    ctx->t = new TupleType(types);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitPrimType(FleetLangParser::PrimTypeContext *ctx) {
    this->visit(ctx->name);
    ctx->t = ctx->name->t;
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitIntType(FleetLangParser::IntTypeContext *ctx) {
    ctx->t = new PrimitiveType(PrimitiveTypeType::Integer);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitFloatType(FleetLangParser::FloatTypeContext *ctx) {
    ctx->t = new PrimitiveType(PrimitiveTypeType::Float);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitBoolType(FleetLangParser::BoolTypeContext *ctx) {
    ctx->t = new PrimitiveType(PrimitiveTypeType::Bool);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitCharType(FleetLangParser::CharTypeContext *ctx) {
    ctx->t = new PrimitiveType(PrimitiveTypeType::Char);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitExpressionStmt(FleetLangParser::ExpressionStmtContext *ctx) {
    this->visit(ctx->expr);
    ctx->stmt = new ExpressionStatement(ctx->expr->e, fromContext(ctx));
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitTupleExpr(FleetLangParser::TupleExprContext *ctx) {
    std::vector<Expression *> exprs;

    for (auto expr : ctx->exprs) {
        this->visit(expr);
        exprs.push_back(expr->e);
    }
    ctx->e = new TupleExpression(exprs);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitStringExpr(FleetLangParser::StringExprContext *ctx) {
    ctx->e = new StringLitExpression(ctx->atom->getText());
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitFloatExpr(FleetLangParser::FloatExprContext *ctx) {
    std::istringstream iss(ctx->atom->getText());
    float value;
    iss >> value;
    if (!iss.good()) ; // TODO Error
    ctx->e = new FloatExpression(value);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitIntExpr(FleetLangParser::IntExprContext *ctx) {
    std::istringstream iss(ctx->atom->getText());
    int32_t value;
    iss >> value;
    if (!iss.good()) ; // TODO Error
    ctx->e = new IntLitExpression(value);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitSignedExpression(FleetLangParser::SignedExpressionContext *ctx) {
    this->visit(ctx->expr);
    ctx->e = new SignedExpression(ctx->sign->getType() == FleetLangLexer::ADD ? ExprSign::Positive : ExprSign::Negative, ctx->expr->e);
    return nullptr;
}

antlrcpp::Any InMemoryVisitor::visitIdExpr(FleetLangParser::IdExprContext *ctx) {
    ctx->e = new IdExpression(ctx->atom->getText());
    return nullptr;
}


