#ifndef PREPROCESSOR_PREPROCESSOR_H
#define PREPROCESSOR_PREPROCESSOR_H

#include "Cpp14Lexer.h"
#include "Cpp14Parser.h"
#include "DefineLexer.h"
#include "DefineParser.h"
#include "DefineCollector.h"

#include <antlr4-runtime.h>

#include <fstream>
#include <memory>
#include <string>

class Preprocessor {
public:
    explicit Preprocessor(std::ifstream& in);
    explicit Preprocessor(const std::string& str);
    Preprocessor(
        const std::string& str,
        DefineCollector::DefineList dList,
        std::unordered_set<std::string> removedMacros
    );

    std::string run();
    bool didReplaceFunction() const;
private:
    void initAntlrEntities(const std::string& str);

    std::unique_ptr<antlr4::ANTLRInputStream> cppInputStream;
    std::unique_ptr<antlr4::ANTLRInputStream> defineInputStream;
    std::unique_ptr<antlr4::CommonTokenStream> cppTokens;
    std::unique_ptr<antlr4::CommonTokenStream> defineTokens;
    std::unique_ptr<Cpp14Lexer> cpp14Lexer;
    std::unique_ptr<Cpp14Parser> cpp14Parser;
    std::unique_ptr<DefineLexer> defineLexer;
    std::unique_ptr<DefineParser> defineParser;

    bool hasInitDefines;
    bool replacedFunctionFlag;
    DefineCollector::DefineList defineList;
    std::unordered_set<std::string> removedMacros;
};


#endif //PREPROCESSOR_PREPROCESSOR_H
