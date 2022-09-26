#ifndef PREPROCESSOR_PREPROCESSORVISITOR_H
#define PREPROCESSOR_PREPROCESSORVISITOR_H

#include "Cpp14ParserBaseVisitor.h"
#include "Cpp14Parser.h"
#include "DefineCollector.h"

#include <any>
#include <memory>
#include <vector>
#include <unordered_map>
#include <sstream>

class PreprocessorVisitor : public Cpp14ParserBaseVisitor {
public:
    PreprocessorVisitor() = delete;
    PreprocessorVisitor(
            const DefineCollector::DefineList& definesList,
            bool hasInitDefines,
            std::unordered_set<std::string> removedMacros);
    std::any visitMultilineMacro(Cpp14Parser::MultilineMacroContext *ctx) override;
    std::any visitDirective(Cpp14Parser::DirectiveContext *ctx) override;
    std::any visitIdentifier(Cpp14Parser::IdentifierContext *ctx) override;
    std::any visitFunctionCall(Cpp14Parser::FunctionCallContext *ctx) override;

    //print to stringstream
    std::any visitTerminal(antlr4::tree::TerminalNode* node) override;

    std::string getResult() const;

    bool didReplaceFunction() const;
private:
    bool isObjectDefine(const std::string& identifier) const;
    std::string getObjectReplacement(antlr4::ParserRuleContext* ctx);

    std::vector<std::string> getArgumentList(Cpp14Parser::ArgumentListContext *ctx);
    std::string launchRecursively(std::string text);
    std::string launchOnce(std::string text);

    bool isFunctionDefine(antlr4::ParserRuleContext* ctx) const;
    static std::string getFunctionBodyReplacement(
        const DefineFunctionMacro& macro,
        std::vector <std::string> arguments
    );
public:
    using ObjectMacros = std::unordered_map<std::string, DefineObjectMacro>;
    using FunctionMacros = std::unordered_map<std::string, DefineFunctionMacro>;
private:
    ObjectMacros objectMacros;
    FunctionMacros functionMacros;
    std::unordered_set<std::string> removedMacros;
    void addNextDefine(bool expand);
    std::ostringstream oss;
    DefineCollector::DefineList definesList;
    size_t it = 0;
    bool replaceFunctionFlag = false;
};


#endif //PREPROCESSOR_PREPROCESSORVISITOR_H
