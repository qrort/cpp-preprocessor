#ifndef PREPROCESSOR_DEFINECOLLECTOR_H
#define PREPROCESSOR_DEFINECOLLECTOR_H

#include "DefineParserBaseVisitor.h"
#include "DefineParser.h"
#include "DefineMacro.h"

#include <any>
#include <memory>
#include <vector>
#include <unordered_map>
#include <variant>


class DefineCollector : public DefineParserBaseVisitor {
    std::any visitIdentifierList(DefineParser::IdentifierListContext *ctx) override;

    std::any visitReplacementList(DefineParser::ReplacementListContext *ctx) override;

    std::any visitObjectMacro(DefineParser::ObjectMacroContext *ctx) override;

    std::any visitFunctionNoVarargMacro(DefineParser::FunctionNoVarargMacroContext *ctx) override;

    std::any visitFunctionOnlyVarargMacro(DefineParser::FunctionOnlyVarargMacroContext *ctx) override;

    std::any visitFunctionVarargMacro(DefineParser::FunctionVarargMacroContext *ctx) override;

public:
    using DefineMacro = std::variant<DefineObjectMacro, DefineFunctionMacro>;
    using DefineList = std::vector<DefineMacro>;
    std::any visitDefinesList(DefineParser::DefinesListContext *ctx) override;
};


#endif //PREPROCESSOR_DEFINECOLLECTOR_H
