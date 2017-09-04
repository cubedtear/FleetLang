// Created by aritz.

#include "CodegenHelper.h"
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <fstream>


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


    TheFPM->add(llvm::createInstructionCombiningPass());
    TheFPM->add(llvm::createDeadCodeEliminationPass());
    TheFPM->add(llvm::createReassociatePass());
    TheFPM->add(llvm::createGVNPass());
    TheFPM->add(llvm::createCFGSimplificationPass());

    TheFPM->doInitialization();
}

void WriteOBJ(std::string Filename) {
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
    if (Target == nullptr) {
        errs() << Error;
        return;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TheTargetMachine = std::unique_ptr<llvm::TargetMachine>(
            Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM));

    TheModule->setDataLayout(TheTargetMachine->createDataLayout());

    if (Filename.empty()) {
        char tmpname[L_tmpnam];
        if (std::tmpnam(tmpname) != nullptr) {
            std::error_code EC;
            raw_fd_ostream dest(tmpname, EC, sys::fs::F_None);

            if (EC) {
                errs() << "Could not open file: " << EC.message();
                return;
            }

            legacy::PassManager pass;
            auto FileType = TargetMachine::CGFT_ObjectFile;

            if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
                errs() << "The target machine can't emit a file of this type";
                return;
            }

            pass.run(*TheModule);
            dest.flush();

            std::string line;
            std::ifstream openfile(tmpname);
            if (openfile.is_open()) {
                while (!openfile.eof()) {
                    getline(openfile, line);
                    std::cout << line << std::endl;
                }
            }
        } else {
            errs() << "Could not get temporary file";
        }
    } else {
        std::error_code EC;
        raw_fd_ostream dest(Filename, EC, sys::fs::F_None);

        if (EC) {
            errs() << "Could not open file: " << EC.message();
            return;
        }

        legacy::PassManager pass;
        auto FileType = TargetMachine::CGFT_ObjectFile;

        if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
            errs() << "The target machine can't emit a file of this type";
            return;
        }

        pass.run(*TheModule);
        dest.flush();
    }

    //outs() << "Wrote " << Filename << "\n";


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

llvm::Type *GetInt8Type() {
    return llvm::Type::getInt8Ty(TheContext);
}

llvm::Type *GetInt16Type() {
    return llvm::Type::getInt16Ty(TheContext);
}

llvm::Type *GetInt64Type() {
    return llvm::Type::getInt64Ty(TheContext);
}

llvm::Type *GetDoubleType() {
    return llvm::Type::getDoubleTy(TheContext);
}

llvm::Type *GetUInt8Type() {
    return llvm::Type::getInt8Ty(TheContext);
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
        case Type::Byte:
            return GetInt8Type();
        case Type::Char:
            return GetUInt8Type();
        case Type::Short:
            return GetInt16Type();
        case Type::Long:
            return GetInt64Type();
        case Type::Double:
            return GetDoubleType();
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
        case Type::Byte:
            return "byte";
        case Type::Char:
            return "char";
        case Type::Short:
            return "short";
        case Type::Long:
            return "long";
        case Type::Double:
            return "double";
    }
    throw "Unknown type";
}

llvm::AllocaInst *FindAlloca(std::string name) {
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    std::string varName = TheFunction->getName().str() + name;

    const auto &iterator = AllocaValues.find(name);
    if (iterator == AllocaValues.end()) {
        const auto &iterator = AllocaValues.find(varName);
        if (iterator == AllocaValues.end()) return nullptr;
        else return iterator->second;
    } else return iterator->second;
}

llvm::AllocaInst *CreateAlloca(Type type, std::string name) {
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());

    std::string varName = TheFunction->getName().str() + name;

    if (AllocaValues.find(varName) != AllocaValues.end()) {
        return (llvm::AllocaInst *) LogErrorV("Variable redeclared!");
    }

    llvm::AllocaInst *alloca = TmpB.CreateAlloca(GetFromType(type), 0, varName);

    AllocaValues.insert(std::make_pair(varName, alloca));
    return alloca;
}
