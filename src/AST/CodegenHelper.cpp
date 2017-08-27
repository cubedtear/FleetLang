// Created by aritz.

#include "CodegenHelper.h"
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>


llvm::LLVMContext TheContext;

llvm::IRBuilder<> Builder(TheContext);

// Create a new pass manager attached to it.
std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(
        TheModule.get());

std::unique_ptr<llvm::Module> TheModule = llvm::make_unique<llvm::Module>("FleetLang.fl", TheContext);
std::map<std::string, llvm::Value *> NamedValues;
std::map<std::string, llvm::AllocaInst *> AllocaValues;


llvm::Value *LogErrorV(const char *str) {
    std::cerr << "Error: " << str << std::endl;
    return nullptr;
}

void InitializeOptimizations() {
    using namespace llvm;
    using namespace llvm::sys;

    // Do simple "peephole" optimizations and bit-twiddling optzns.
    TheFPM->add(llvm::createInstructionCombiningPass());
    TheFPM->add(llvm::createDeadCodeEliminationPass());
    // Reassociate expressions.
    TheFPM->add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
    TheFPM->add(llvm::createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    TheFPM->add(llvm::createCFGSimplificationPass());

    TheFPM->doInitialization();
}

void WriteOBJ() {
    using namespace llvm;
    using namespace llvm::sys;

    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto TargetTriple = sys::getDefaultTargetTriple();
    TheModule->setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        errs() << "Hola!\n";
        errs() << Error;
        return;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TheTargetMachine =
            Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    TheModule->setDataLayout(TheTargetMachine->createDataLayout());

    auto Filename = "output.o";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::F_None);

    if (EC) {
        errs() << "Adios!\n";
        errs() << "Could not open file: " << EC.message();
        return;
    }

    legacy::PassManager pass;
    auto FileType = TargetMachine::CGFT_ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
        errs() << "Wat!\n";
        errs() << "TheTargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*TheModule);
    dest.flush();

    outs() << "Wrote " << Filename << "\n";


}

llvm::Type *GetFloatType() {
    return llvm::Type::getFloatTy(TheContext);
}

llvm::Type *GetInt32Type() {
    return llvm::Type::getInt32Ty(TheContext);
}

llvm::Type *GetBoolType() {
    return llvm::Type::getInt1Ty(TheContext);
}

llvm::Type *GetVoidType() {
    return llvm::Type::getVoidTy(TheContext);
}

llvm::Type *GetFromType(Type type) {
    switch (type) {
        case Type::Int:
            return GetInt32Type();
        case Type::Float:
            return GetFloatType();
        case Type::Bool:
            return GetBoolType();
        case Type::Void:
            return GetVoidType();
    }
    throw "Unknown type";
}

std::string TypeToString(Type type) {
    switch (type) {
        case Type::Int:
            return "int";
        case Type::Float:
            return "float";
        case Type::Bool:
            return "bool";
        case Type::Void:
            return "void";
    }
    throw "Unknown type";
}

llvm::AllocaInst *FindAlloca(std::string name) {
    const std::map<std::string, llvm::AllocaInst *>::iterator &iterator = AllocaValues.find(name);
    if (iterator == AllocaValues.end()) return nullptr;
    else return iterator->second;
}

llvm::AllocaInst *CreateAlloca(Type type, std::string name) {
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());

    if (AllocaValues.find(name) != AllocaValues.end()) {
        return (llvm::AllocaInst *) LogErrorV("Variable redeclared!");
    }

    llvm::AllocaInst *alloca = TmpB.CreateAlloca(GetFromType(type), 0, name);

    AllocaValues.insert(std::make_pair(name, alloca));
    return alloca;
}
