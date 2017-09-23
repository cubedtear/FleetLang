#include <iostream>
#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <FleetLangParser.h>
#include <FleetLangLexer.h>
#include "NewVisitor.h"
#include "AST/CodegenHelper.h"
#include "AST/Program.h"
#include "argh.h"
#include "Validator.h"

int main(int argc, const char *const *argv) {
    using namespace antlr4;

    std::unique_ptr<ANTLRInputStream> is;

	if (argc <= 1) {
		std::cout << "Usage: " << argv[0] << " <input file> [-o outputfile] [-v]" << std::endl;
		return 0;
	}

    argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION | argh::parser::SINGLE_DASH_IS_MULTIFLAG);
    if (cmdl(0)) {
        std::ifstream stream(argv[1]);
        is = std::make_unique<ANTLRInputStream>(stream);
    } else {
        is = std::make_unique<ANTLRInputStream>(std::cin);
    }

    InitializeOptimizations();

    FleetLangLexer l(is.get());
    CommonTokenStream ts(&l);
    FleetLangParser p(&ts);
    NewVisitor v;
    FleetLangParser::ProgramContext *root = p.program();
    size_t errors = p.getNumberOfSyntaxErrors();
    if (errors > 0) {
        return 1;
    }
    Program *pr = v.visit(root).as<Program *>();
    Validator validator;
    bool isValid = validator.check(pr);
    if (isValid) {
//    std::cout << pr->print() << std::endl;
        pr->generate();

        if (cmdl["v"]) TheModule->print(llvm::outs(), nullptr);


        WriteOBJ(cmdl("o") ? cmdl("o").str() : std::string(""));
    } else {
        std::cerr << "Error compiling" << std::endl;
    }

    delete pr;
    return 0;
}