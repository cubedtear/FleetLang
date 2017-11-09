// Created by aritz.

#pragma once

#include <iostream>
//#include <llvm/IR/Module.h>
//#include <llvm/IR/LLVMContext.h>
//#include <llvm/IR/IRBuilder.h>
//#include <llvm/Support/raw_ostream.h>
//#include <llvm/IR/PassManager.h>
//#include "llvm/Transforms/Scalar.h"
//#include "llvm/Transforms/Scalar/GVN.h"

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include "StmtAST.h"

extern llvm::LLVMContext TheContext;

extern llvm::IRBuilder<> Builder;

extern std::unique_ptr<llvm::Module> TheModule;
extern std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
extern std::map<std::string, llvm::Value *> NamedValues;
extern std::map<std::string, llvm::AllocaInst *> AllocaValues;

llvm::Value *LogErrorV(const char *str);

llvm::Type *GetFloatType();

llvm::Type *GetInt32Type();

llvm::Type *GetBoolType();

llvm::Type *GetVoidType();

llvm::Type *GetFromType(Type type);

std::string TypeToString(Type type);

llvm::AllocaInst *FindAlloca(std::string name);

llvm::AllocaInst *CreateAlloca(Type type, std::string name);

void InitializeOptimizations();

void WriteOBJ(std::string Filename);


