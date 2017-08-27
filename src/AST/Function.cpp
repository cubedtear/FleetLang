// Created by aritz.

#include <sstream>
#include <llvm/IR/Verifier.h>
#include "Function.h"

Function::Function(Type ret, std::string name, std::vector<std::pair<std::string, Type>> args,
                   std::vector<std::unique_ptr<StmtAST>> stmts)
        : FunctionDeclaration(ret, std::move(name), std::move(args)), stmts(std::move(stmts)) {}

std::string Function::print() {

    std::stringstream ss;
    ss << this->name << "(";
    for (unsigned int i = 0; i < this->args.size() - 1; i++) {
        ss << TypeToString(this->args[i].second) << " " << this->args[i].first << ", ";
    }
    if (!this->args.empty())
        ss << TypeToString(this->args[this->args.size() - 1].second) << " " << this->args[this->args.size() - 1].first;
    ss << ") {\n";

    for (auto &s : this->stmts) {
        ss << s->print();
    }

    ss << "}\n";

    return ss.str();
}

llvm::Function *Function::generate() {
    llvm::Function *TheFunction = TheModule->getFunction(this->name);

    if (TheFunction == nullptr) { // If not declared, declare it
        TheFunction = FunctionDeclaration::generate();
    }

    if (TheFunction == nullptr)
        return nullptr;

    if (!TheFunction->empty())
        return (llvm::Function *) LogErrorV("Function cannot be redefined.");

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", TheFunction);
    Builder.SetInsertPoint(BB);

    // Record the function arguments in the NamedValues map.
    NamedValues.clear();
    for (auto &arg : TheFunction->args())
        NamedValues[arg.getName()] = &arg;

    for (auto &stmt : this->stmts) {
        stmt->generate();
    }

    // Add terminators to blocks that do not have one
    const auto &originalPoint = Builder.GetInsertPoint();
    llvm::BasicBlock *originalBlock = Builder.GetInsertBlock();
    for (auto &b : TheFunction->getBasicBlockList()) {
        if (b.getTerminator() == nullptr) {
            Builder.SetInsertPoint(&b);
            Builder.CreateUnreachable();
        }
    }
    Builder.SetInsertPoint(originalBlock, originalPoint);

    llvm::verifyFunction(*TheFunction, &llvm::errs());
    TheFPM->run(*TheFunction);
//    TheFunction->viewCFG();

    return TheFunction;
}

FunctionDeclaration::FunctionDeclaration(Type ret, std::string name, std::vector<std::pair<std::string, Type>> args)
        : ret(ret), name(std::move(name)), args(std::move(args)) {}

llvm::Function *FunctionDeclaration::generate() {
    std::vector<llvm::Type *> ArgTypes;
    for (auto &p : this->args) {
        ArgTypes.push_back(GetFromType(p.second));
    }
    llvm::FunctionType *FT = llvm::FunctionType::get(GetFromType(this->ret), ArgTypes, false);

    llvm::Function *TheFunction = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, this->name,
                                                         TheModule.get());

    unsigned Idx = 0;
    for (auto &arg : TheFunction->args())
        arg.setName(this->args[Idx++].first);
    return TheFunction;
}

std::string FunctionDeclaration::print() {
    std::stringstream ss;
    switch (this->ret) {
        case Type::Int:
            ss << "int ";
            break;
        case Type::Float:
            ss << "float ";
            break;
        case Type::Bool:
            ss << "bool ";
            break;
        case Type::Void:
            ss << "void ";
            break;
    }
    ss << this->name << "(";
    for (unsigned int i = 0; i < this->args.size() - 1; i++) {
        ss << TypeToString(this->args[i].second) << " " << this->args[i].first << ", ";
    }
    if (!this->args.empty())
        ss << TypeToString(this->args[this->args.size() - 1].second) << " " << this->args[this->args.size() - 1].first;
    ss << ");\n";
    return ss.str();
}
