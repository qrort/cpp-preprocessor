#include "Preprocessor.h"

/*

java -jar ../antlr/antlr-4.10.1-complete.jar -Dlanguage=Cpp ../grammar/DefineLexer.g4 ../grammar/DefineParser.g4 -no-listener -visitor

java -jar ../antlr/antlr-4.10.1-complete.jar -Dlanguage=Cpp ../grammar/Cpp14Lexer.g4 ../grammar/Cpp14Parser.g4 -no-listener -visitor

 */

#include "antlr4-runtime.h"

int main() {
    std::ifstream in("input.txt");

    Preprocessor preprocessor(in);

    std::string output = preprocessor.run();

    std::cout << output;
//    std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

    return 0;
}