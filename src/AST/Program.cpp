// Created by aritz.

#include <sstream>
#include "Program.h"

Program::Program(std::vector<std::unique_ptr<FunctionDeclaration>> functions)
        : functions(std::move(functions)) {}

std::string Program::print() {
    std::stringstream ss;
    for (auto &f : this->functions) {
        ss << f->print() << "\n";
    }
    return ss.str();
}

void Program::generate() {
    for (auto &f : this->functions) {
        f->generate();
    }
}

const std::vector<std::unique_ptr<FunctionDeclaration>> &Program::GetFunctions() {
    return this->functions;
}
