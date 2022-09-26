#include "DefineCollector.h"

std::any DefineCollector::visitObjectMacro(DefineParser::ObjectMacroContext *ctx) {
    auto res = DefineObjectMacro(
        ctx->identifier()->getText(),
        ctx->replacementList()->getText()
    );
    return res;
}

std::any DefineCollector::visitFunctionNoVarargMacro(DefineParser::FunctionNoVarargMacroContext *ctx) {
    auto arguments = std::any_cast<std::vector <std::string>>(visitIdentifierList(ctx->identifierList()));
    auto replacement = std::any_cast<std::vector <std::string>>(visitReplacementList(ctx->replacementList()));
    auto res = DefineFunctionMacro(
        ctx->identifier()->getText(),
        replacement,
        arguments,
        false
    );
    return res;
}

std::any DefineCollector::visitFunctionOnlyVarargMacro(DefineParser::FunctionOnlyVarargMacroContext *ctx) {
    auto replacement = std::any_cast<std::vector <std::string>>(visitReplacementList(ctx->replacementList()));
    auto res = DefineFunctionMacro(
        ctx->identifier()->getText(),
        replacement,
        std::vector<std::string>(),
        true
    );
    return res;
}

std::any DefineCollector::visitFunctionVarargMacro(DefineParser::FunctionVarargMacroContext *ctx) {
    auto arguments = std::any_cast<std::vector <std::string>>(visitIdentifierList(ctx->identifierList()));
    auto replacement = std::any_cast<std::vector <std::string>>(visitReplacementList(ctx->replacementList()));
    auto res = DefineFunctionMacro(
            ctx->identifier()->getText(),
            replacement,
            arguments,
            true
    );
    return res;
}

std::any DefineCollector::visitIdentifierList(DefineParser::IdentifierListContext *ctx) {
    if (!ctx) {
        return std::vector<std::string>();
    }
    std::vector <std::string> arguments = {ctx->identifier()->getText()};
    if (ctx->identifierList()) {
        auto tail = std::any_cast<std::vector <std::string>>(visitIdentifierList(ctx->identifierList()));
        for (auto& arg : tail) {
            arguments.emplace_back(std::move(arg));
        }
    }
    return arguments;
}

std::any DefineCollector::visitReplacementList(DefineParser::ReplacementListContext *ctx) {
    std::vector <std::string> replacement;
    if (ctx->ppTokens()) {
        for (auto& pptoken : ctx->ppTokens()->ppToken()) {
            replacement.emplace_back(pptoken->getText());
        }
    };
    if (ctx->replacementList()) {
        auto tail = std::any_cast<std::vector <std::string>>(visitReplacementList(ctx->replacementList()));
        replacement.emplace_back(ctx->functionReplacementDelimiter()->getText());
        for (auto& token : tail) {
            replacement.emplace_back(std::move(token));
        }
    }
    return replacement;
}

std::any DefineCollector::visitDefinesList(DefineParser::DefinesListContext *ctx) {
    DefineList result;
    for (auto* controlLine : ctx->controlLine()) {
        std::any defineMacro = visitControlLine(controlLine);
        try {
            result.emplace_back(std::move(std::any_cast<DefineObjectMacro>(defineMacro)));
        } catch (...) {
            result.emplace_back(std::move(std::any_cast<DefineFunctionMacro>(defineMacro)));
        }
    }
    return result;
}

