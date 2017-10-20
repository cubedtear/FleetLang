// Created by aritz.

#include "Validator.h"


Validator::Validator() {
    this->table = new SymbolTable();
}

Validator::~Validator() {
    delete this->table;
}

void Validator::pushScope() {
    this->table = new SymbolTable(this->table);
}

void Validator::popScope() {
    this->table = this->table->GetParent();
}

bool Validator::check(Program *p) {
    for (const auto &f : p->GetFunctions()) {
        auto *func = dynamic_cast<Function *>(f.get());
        if (func != nullptr) {
            if (!this->check(func)) {
                return false;
            }
        } else {
            if (!this->check(f.get())) {
                return false;
            }
        }
    }
    return true;
}

bool Validator::check(FunctionDeclaration *f) {
    FunctionDeclaration *prevDecl = this->table->FindFunction(f->GetName());
    if (prevDecl != nullptr && *prevDecl != *f) {
        if (prevDecl->GetReturnType() == f->GetReturnType()) {
            // Overload found
            // TODO Allow overloads, by changing how they are implemented in the symbol table
            return false;
        }
    }
    this->table->addFunction(f);
    return true;
}

bool Validator::check(Function *f) {
    if (!this->check((FunctionDeclaration *) f)) {
        return false;
    }

    this->currentFunction = f;

    this->pushScope();
    for (const auto &arg : f->GetArgs()) {
        if (this->table->FindVariableInCurrentScope(arg->GetName()) != nullptr) {
            this->popScope();
            return false;
        }
        this->table->addVariable(arg.get());
    }

    for (const auto &stmt : f->GetStmts()) {
        if (!this->check(stmt.get())) {
            this->popScope();
            return false;
        }
    }

    this->popScope();

    this->currentFunction = nullptr;

    return true;
}

bool Validator::check(StmtAST *s) {
    if (dynamic_cast<VarDeclStmtAST *>(s) != nullptr) {
        return this->check((VarDeclStmtAST *) s);
    } else if (dynamic_cast<AssignStmtAST *>(s) != nullptr) {
        return this->check((AssignStmtAST *) s);
    } else if (dynamic_cast<IfStmtAST *>(s) != nullptr) {
        return this->check((IfStmtAST *) s);
    } else if (dynamic_cast<WhileStmtAST *>(s) != nullptr) {
        return this->check((WhileStmtAST *) s);
    } else if (dynamic_cast<ReturnStmtAST *>(s) != nullptr) {
        return this->check((ReturnStmtAST *) s);
    } else if (dynamic_cast<ExprStmtAST *>(s) != nullptr) {
        return this->check((ExprStmtAST *) s);
    } else if (dynamic_cast<BlockStmtAST *>(s) != nullptr) {
        return this->check((BlockStmtAST *) s);
    } else if (dynamic_cast<VarDeclStmtAST *>(s) != nullptr) {
        return this->check((VarDeclStmtAST *) s);
    }
    return false;
}

bool Validator::check(AssignStmtAST *s) {
    if (s->GetDecl() != nullptr) {
        this->check(s->GetDecl());
    }
    if (this->table->FindVariable(s->GetLVar()) == nullptr) {
        return false;
    }
    return true;
}

bool Validator::check(IfStmtAST *s) {
    auto c = this->check(s->GetCond());
    if (!c.first) return false;

    if (c.second != Type::Bool) {
        return false;
    }
    if (s->GetTrue() != nullptr) {
        this->pushScope();
        if (!this->check(s->GetTrue())) {
            this->popScope();
            return false;
        }
        this->popScope();
    }
    if (s->GetFalse() != nullptr) {
        this->pushScope();
        if (!this->check(s->GetFalse())) {
            this->popScope();
            return false;
        }
        this->popScope();
    }
    return true;
}

bool Validator::check(WhileStmtAST *s) {
    auto c = this->check(s->GetCond());
    if (!c.first) return false;
    if (c.second != Type::Bool) {
        return false;
    }

    if (s->GetBody() != nullptr) {
        this->pushScope();
        if (!this->check(s->GetBody())) {
            this->popScope();
            return false;
        }
        this->popScope();
    }
    return true;
}

bool Validator::check(ReturnStmtAST *s) {
    if (s->GetExpr() == nullptr) {
        if (currentFunction->GetReturnType() != Type::Void) {
            return false;
        } else {
            return true;
        }
    }
    auto val = this->check(s->GetExpr());
    if (!val.first) return true;
    if (val.second != currentFunction->GetReturnType()) {
        return false;
    }
    return true;
}

bool Validator::check(ExprStmtAST *s) {
    return this->check(s->GetExpr()).first;
}

bool Validator::check(BlockStmtAST *s) {
    this->pushScope();
    for (const auto &stmt : s->GetStmts()) {
        if (!this->check(stmt.get())) {
            this->popScope();
            return false;
        }
    }
    this->popScope();
    return true;
}

bool Validator::check(VarDeclStmtAST *s) {
    auto var = this->table->FindVariableInCurrentScope(s->GetName());
    if (var != nullptr) {
        return false;
    }
    this->table->addVariable(s);
    return true;
}

std::pair<bool, Type> Validator::check(ExprAST *e) {
    if (dynamic_cast<StringExprAST *>(e) != nullptr) {
        return this->check((StringExprAST *) e);
    } else if (dynamic_cast<FloatExprAST *>(e) != nullptr) {
        return this->check((FloatExprAST *) e);
    } else if (dynamic_cast<DoubleExprAST *>(e) != nullptr) {
        return this->check((DoubleExprAST *) e);
    } else if (dynamic_cast<IntExprAST *>(e) != nullptr) {
        return this->check((IntExprAST *) e);
    } else if (dynamic_cast<BinaryExprAST *>(e) != nullptr) {
        return this->check((BinaryExprAST *) e);
    } else if (dynamic_cast<UnaryExprAST *>(e) != nullptr) {
        return this->check((UnaryExprAST *) e);
    } else if (dynamic_cast<CallExprAST *>(e) != nullptr) {
        return this->check((CallExprAST *) e);
    } else if (dynamic_cast<VariableExprAST *>(e) != nullptr) {
        return this->check((VariableExprAST *) e);
    } else if (dynamic_cast<ComparisonExprAST *>(e) != nullptr) {
        return this->check((ComparisonExprAST *) e);
    } else if (dynamic_cast<BoolOpExprAST *>(e) != nullptr) {
        return this->check((BoolOpExprAST *) e);
    } else if (dynamic_cast<LiteralBoolExprAST *>(e) != nullptr) {
        return this->check((LiteralBoolExprAST *) e);
    } else if (dynamic_cast<CastingExprAST *>(e) != nullptr) {
        return this->check((CastingExprAST *) e);
    }
    return std::make_pair(false, Type::Void);
}

std::pair<bool, Type> Validator::check(StringExprAST *e) {
    return std::make_pair(false, Type::Void); // TODO Strings not supported yet
}

std::pair<bool, Type> Validator::check(FloatExprAST *e) {
    return std::make_pair(true, Type::Float);
}

std::pair<bool, Type> Validator::check(DoubleExprAST *e) {
    return std::make_pair(true, Type::Double);
}

std::pair<bool, Type> Validator::check(IntExprAST *e) {
    return std::make_pair(true, Type::Int);
}

std::pair<bool, Type> Validator::check(LiteralBoolExprAST *e) {
    return std::make_pair(true, Type::Bool);
}

std::pair<bool, Type> Validator::check(BinaryExprAST *e) {
    auto lhs = this->check(e->GetLHS());
    if (!lhs.first) return std::make_pair(false, Type::Void);
    auto rhs = this->check(e->GetRHS());
    if (!rhs.first) return std::make_pair(false, Type::Void);

    Type coercion = CanCoerce(lhs.second, rhs.second);
    if (coercion == Type::Void) {
        return std::make_pair(false, Type::Void);
    } else {
        if (coercion != lhs.second) {
            CastingExprAST *ce = new CastingExprAST(std::unique_ptr<ExprAST>(e->GetLHS()), coercion);
            e->SetLHS(ce);
        }
        if (coercion != rhs.second) {
            CastingExprAST *ce = new CastingExprAST(std::unique_ptr<ExprAST>(e->GetRHS()), coercion);
            e->SetRHS(ce);
        }
        return std::make_pair(true, coercion);
    }
}

std::pair<bool, Type> Validator::check(UnaryExprAST *e) {
    auto expr = this->check(e->GetExpr());
    if (!expr.first) return std::make_pair(false, Type::Void);
    if (e->GetOp() == UnaryOp::Not) {
        if (expr.second != Type::Bool) {
            return std::make_pair(false, Type::Void);
        }
        return std::make_pair(true, Type::Bool);
    } else {
        if (expr.second == Type::Bool || expr.second == Type::Void) {
            return std::make_pair(false, Type::Void);
        }
        return std::make_pair(true, expr.second);
    }
}

std::pair<bool, Type> Validator::check(CallExprAST *e) {
    FunctionDeclaration *f = this->table->FindFunction(e->GetCallee());
    if (f == nullptr) {
        return std::make_pair(false, Type::Void);
    }
    if (e->GetArgs().size() != f->GetArgs().size()) {
        return std::make_pair(false, Type::Void);
    }
    for (unsigned int i = 0; i < f->GetArgs().size(); i++) {
        auto a = this->check(e->GetArgs()[i].get());
        if (!a.first) return std::make_pair(false, Type::Void);

        Type coercion = CanCoerce(a.second, f->GetArgs()[i]->GetType());
        if (coercion == Type::Void) {
            return std::make_pair(false, Type::Void);
        }
    }
    return std::make_pair(true, f->GetReturnType());
}

std::pair<bool, Type> Validator::check(VariableExprAST *e) {
    auto var = this->table->FindVariable(e->GetName());
    if (var == nullptr) {
        return std::make_pair(false, Type::Void);
    }
    return std::make_pair(true, var->GetType());
}

std::pair<bool, Type> Validator::check(ComparisonExprAST *e) {
    auto lhs = this->check(e->GetLHS());
    if (!lhs.first) return std::make_pair(false, Type::Void);

    auto rhs = this->check(e->GetRHS());
    if (!rhs.first) return std::make_pair(false, Type::Void);

    Type coercion = CanCoerce(lhs.second, rhs.second);

    if (coercion == Type::Void) {
        return std::make_pair(false, Type::Void);
    } else if (coercion != lhs.second || coercion != rhs.second) {
        if (coercion != lhs.second) {
            CastingExprAST *ce = new CastingExprAST(std::unique_ptr<ExprAST>(e->GetLHS()), coercion);
            e->SetLHS(ce);
        }
        if (coercion != rhs.second) {
            CastingExprAST *ce = new CastingExprAST(std::unique_ptr<ExprAST>(e->GetRHS()), coercion);
            e->SetRHS(ce);
        }
        return this->check(e);
    } else return std::make_pair(true, Type::Bool);
}

std::pair<bool, Type> Validator::check(BoolOpExprAST *e) {
    auto lhs = this->check(e->GetLHS());
    if (!lhs.first) return std::make_pair(false, Type::Void);
    if (lhs.second != Type::Bool) {
        return std::make_pair(false, Type::Void);
    }

    auto rhs = this->check(e->GetRHS());
    if (!rhs.first) return std::make_pair(false, Type::Void);
    if (rhs.second != Type::Bool) {
        return std::make_pair(false, Type::Void);
    }

    return std::make_pair(true, Type::Bool);
}

Type Validator::CanCoerce(Type a, Type b) {
    return TypeCoercions[to_underlying(a)][to_underlying(b)];
}

Type Validator::SmallestType(Type a, Type b) {
    return TypeSizes[to_underlying(a)] > TypeSizes[to_underlying(b)] ? b : a;
}

std::pair<bool, Type> Validator::check(CastingExprAST *e) {
    auto val = this->check(e->GetValue());
    if (!val.first) return std::make_pair(false, Type::Void);
    return std::make_pair(true, e->GetType());
}
