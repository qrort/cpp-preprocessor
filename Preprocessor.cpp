#include "Preprocessor.h"

#include "Cpp14Lexer.h"
#include "Cpp14Parser.h"
#include "DefineCollector.h"
#include "PreprocessorVisitor.h"

#include <antlr4-runtime.h>

#include <sstream>
#include <utility>

namespace {
    std::string readStream(std::ifstream &in) {
        std::ostringstream sstr;
        sstr << in.rdbuf();
        std::string res = sstr.str();
        if (res.back() != '\n') res.push_back('\n');
        return res;
    }

    std::string flattenMultilineMacro(std::string&& text) {
        std::string res;
        for (int i = 0; i < text.size(); i++) {
            if (text[i] == '\\' && i + 1 < text.size() && text[i + 1] == '\n') {
                i++;
            } else {
                res.push_back(text[i]);
            }
        }
        return res;
    }
}

void Preprocessor::initAntlrEntities(const std::string &str) {
    cppInputStream = std::make_unique<antlr4::ANTLRInputStream>(str);
    cpp14Lexer = std::make_unique<Cpp14Lexer>(cppInputStream.get());
    cppTokens = std::make_unique<antlr4::CommonTokenStream>(cpp14Lexer.get());
    cppTokens->fill();
    cpp14Parser = std::make_unique<Cpp14Parser>(cppTokens.get());
    if (!hasInitDefines) {
        std::string rawDefines;
        for (auto *token: cppTokens->getTokens()) {
            //std::cout << token->getTokenIndex() << ' ' << token->getType() << ' ' << token->getText() << std::endl;
            if (
                    token->getType() == Cpp14Lexer::MultiLineMacro
                    || token->getType() == Cpp14Lexer::Directive
                    ) {
                if (token->getType() == Cpp14Lexer::MultiLineMacro) {
                    rawDefines += flattenMultilineMacro(token->getText());
                } else {
                    rawDefines += token->getText();
                }
                rawDefines += '\n';
            }
        }
        //
        defineInputStream = std::make_unique<antlr4::ANTLRInputStream>(rawDefines);
        defineLexer = std::make_unique<DefineLexer>(defineInputStream.get());
        defineTokens = std::make_unique<antlr4::CommonTokenStream>(defineLexer.get());
        defineTokens->fill();
        defineParser = std::make_unique<DefineParser>(defineTokens.get());
    }
}

Preprocessor::Preprocessor(const std::string& str)
    : hasInitDefines(false)
{
    initAntlrEntities(str);
}


Preprocessor::Preprocessor(std::ifstream &in)
    : hasInitDefines(false)
{
    initAntlrEntities(readStream(in));
}


Preprocessor::Preprocessor(
    const std::string& str,
    DefineCollector::DefineList dList,
    std::unordered_set<std::string>  removedMacros
)
    : hasInitDefines(true)
    , removedMacros(std::move(removedMacros))
{
    initAntlrEntities(str);
    defineList = std::move(dList);
}

std::string Preprocessor::run() {
    if (!hasInitDefines) {
        std::vector<std::unique_ptr<DefineMacro>> defineMacros;
        DefineParser::DefinesListContext *definesList = defineParser->definesList();
        DefineCollector defineCollector;
        defineList = std::any_cast<DefineCollector::DefineList>(defineCollector.visitDefinesList(definesList));
    }
    std::cerr << defineList.size() << '\n';
    PreprocessorVisitor preprocessorVisitor(
        defineList,
        hasInitDefines,
        std::move(removedMacros)
    );
    Cpp14Parser::TranslationUnitContext* translationUnitContext = cpp14Parser->translationUnit();
    preprocessorVisitor.visitTranslationUnit(translationUnitContext);
    replacedFunctionFlag = preprocessorVisitor.didReplaceFunction();
    return preprocessorVisitor.getResult();
}

bool Preprocessor::didReplaceFunction() const {
    return replacedFunctionFlag;
}
