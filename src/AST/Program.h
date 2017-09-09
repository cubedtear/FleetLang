// Created by aritz.

#pragma once

#include "Function.h"
#include "CodegenHelper.h"

class Program {
    std::vector<std::unique_ptr<FunctionDeclaration>> functions;

public:
    explicit Program(std::vector<std::unique_ptr<FunctionDeclaration>> functions);
    std::string print();
    void generate();

    const std::vector<std::unique_ptr<FunctionDeclaration>> &GetFunctions();
};


