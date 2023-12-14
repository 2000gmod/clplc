#include <iostream>
#include "parser.hpp"
#include "compiler.hpp"

#include <sstream>
#include <fstream>

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "Usage: [MODE] <INPUT_FILE> <OUTPUT_FILE>\n";
        return 1;
    }
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(std::string(argv[i]));
    }

    if (args[0] == "-h") {
        std::ifstream in(args.at(1));
        std::stringstream buf;
        buf << in.rdbuf();

        clpl::Parser parser(buf.str());
        auto sts = parser.parse();

        std::ofstream out(args.at(2));
        out << clpl::generateDeclarations(sts);
    }
    else {
        std::ifstream in(args.at(0));
        std::stringstream buf;
        buf << in.rdbuf();

        clpl::Parser parser(buf.str());
        auto sts = parser.parse();

        clpl::Compiler compiler(args.at(1).c_str(), sts);
        compiler.compile();
        compiler.output(args.at(1).c_str());
    }

    return 0;
}