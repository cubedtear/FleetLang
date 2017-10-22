#include <iostream>
#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <FleetLangParser.h>
#include <FleetLangLexer.h>
#include "NewVisitor.h"
#include "AST/CodegenHelper.h"
#include "AST/Program.h"
#include "Validator.h"

int main(int argc, const char *const *argv) {
    using namespace antlr4;

    std::unique_ptr<ANTLRInputStream> is;

    if (argc < 2) { // We require the input file, or "-" to read from stdin
        std::cout << "Usage: " << argv[0] << " <input file> [-o outputfile] [-v]" << std::endl;
        return 0;
    }

    std::string input_file = argv[1];
    bool verbose = false;
    std::string output_file;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i == argc) {
                std::cerr << "USAGE" << std::endl;
                return -1;
            } else {
                output_file = argv[i + 1];
            }
        }
    }

    if (input_file == "-") {
        std::istreambuf_iterator<char> eos;
        std::string std_input(std::istreambuf_iterator<char>(std::cin), eos);

        is = std::make_unique<ANTLRInputStream>(std_input);

    } else {
        std::ifstream stream(argv[1]);
        is = std::make_unique<ANTLRInputStream>(stream);
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

        if (verbose) TheModule->print(llvm::outs(), nullptr);


        WriteOBJ(output_file);
    } else {
        std::cerr << "Error compiling" << std::endl;
    }

    delete pr;
    return 0;
}