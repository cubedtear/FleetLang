#include <iostream>
#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <FleetLangParser.h>
#include <FleetLangLexer.h>
#include "NewVisitor.h"
#include "AST/CodegenHelper.h"

int main(int argc, char **argv) {
    using namespace antlr4;

    InitializeOptimizations();

    std::ifstream ifs("input.txt");
    ANTLRInputStream is(ifs);
    FleetLangLexer l(&is);
    CommonTokenStream ts(&l);
    FleetLangParser p(&ts);
//    InMemoryVisitor v;
    NewVisitor v;
    FleetLangParser::ProgramContext *root = p.program();
    size_t errors = p.getNumberOfSyntaxErrors();
    if (errors > 0) {
        return 1;
    }
    Program *pr = v.visit(root).as<Program *>();
    //std::cout << pr->print() << std::endl;
    pr->generate();

    TheModule->print(llvm::outs(), nullptr);

    WriteOBJ();
    return 0;
}