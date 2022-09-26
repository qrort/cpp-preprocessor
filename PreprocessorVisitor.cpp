#include "PreprocessorVisitor.h"
#include "Preprocessor.h"

PreprocessorVisitor::PreprocessorVisitor(
    const DefineCollector::DefineList& definesList,
    bool hasInitDefines,
    std::unordered_set<std::string> removedMacros
)
    : definesList(definesList)
    , removedMacros(std::move(removedMacros))
{
    if (hasInitDefines) {
        for (int i = 0; i < definesList.size(); i++) {
            addNextDefine(false);
        }
    }
}

std::any PreprocessorVisitor::visitMultilineMacro(Cpp14Parser::MultilineMacroContext *ctx) {
    addNextDefine(true);
    return {};
}

std::any PreprocessorVisitor::visitDirective(Cpp14Parser::DirectiveContext *ctx) {
    addNextDefine(true);
    return {};
}

std::string PreprocessorVisitor::launchRecursively(std::string text) {
    std::string res = text;
    do {
        text = res;
        std::vector<DefineCollector::DefineMacro> currentDefines;
        currentDefines.reserve(it);
        std::copy_if(
                definesList.begin(),
                definesList.begin() + it,
                std::back_inserter(currentDefines),
                [&](const DefineCollector::DefineMacro& macro) {
                    if (std::holds_alternative<DefineObjectMacro>(macro)) {
                        return !removedMacros.count(
                            std::get<DefineObjectMacro>(macro).identifier
                        );
                    } else {
                        return !removedMacros.count(
                            std::get<DefineFunctionMacro>(macro).identifier
                        );
                    }
                }
        );
        Preprocessor recursivePreprocessor(text, currentDefines, removedMacros);
        res = recursivePreprocessor.run();
    } while (text != res);
    return res;
}

std::string PreprocessorVisitor::launchOnce(std::string text) {
    std::vector<DefineCollector::DefineMacro> currentDefines;
    currentDefines.reserve(it);
    std::copy(
            definesList.begin(),
            definesList.begin() + it,
            std::back_inserter(currentDefines)
    );
    Preprocessor recursivePreprocessor(text, currentDefines, removedMacros);
    return recursivePreprocessor.run();
}

void PreprocessorVisitor::addNextDefine(bool expand) {
    DefineCollector::DefineMacro define = definesList[it];
    if (std::holds_alternative<DefineObjectMacro>(define)) {
        auto objectMacro = std::get<DefineObjectMacro>(define);
        if (expand) {
            std::cerr << "replacing object macro: " << objectMacro.replacement << '\n';
            objectMacro.replacement = launchRecursively(objectMacro.replacement);
            std::cerr << "done as: " << objectMacro.replacement << '\n';
        }
        objectMacros[objectMacro.identifier] = std::move(objectMacro);
    } else {
        auto functionMacro = std::get<DefineFunctionMacro>(define);
        if (expand) {
            std::string tmp;
            for (const std::string& replacementPart: functionMacro.replacement) {
                tmp += replacementPart;
            }
            std::cerr << "replacing funciton macro: " << tmp << '\n';
            tmp.clear();
            for (std::string& replacementPart: functionMacro.replacement) {
                replacementPart = launchRecursively(replacementPart);
                tmp += replacementPart;
            }
            std::cerr << "done as: " << tmp << '\n';
        }
        functionMacros[functionMacro.identifier] = std::move(functionMacro);
    }
    it++;
}

std::any PreprocessorVisitor::visitTerminal(antlr4::tree::TerminalNode *node) {
    if (node->getText() != "<EOF>") {
        oss << node->getText();
    }
    return {};
}

bool PreprocessorVisitor::isObjectDefine(const std::string& identifier) const {
    return objectMacros.find(identifier) != objectMacros.end();
}

std::string PreprocessorVisitor::getObjectReplacement(antlr4::ParserRuleContext *ctx) {
    return objectMacros[ctx->getText()].replacement;
}

std::any PreprocessorVisitor::visitIdentifier(Cpp14Parser::IdentifierContext *ctx) {
    std::string identifier = ctx->getText();
    if (isObjectDefine(identifier)) {
        std::cerr << "replacing " << identifier << '\n';
        oss << getObjectReplacement(ctx);
        removedMacros.emplace(ctx->getText());
    } else {
        visitChildren(ctx);
    }
    return {};
}

std::any PreprocessorVisitor::visitFunctionCall(Cpp14Parser::FunctionCallContext *ctx) {
    std::cerr << "visited " << ctx->identifier()->getText() << std::endl;
    bool isDefine = isFunctionDefine(ctx->identifier());
    if (isDefine) {
        const DefineFunctionMacro& macro = functionMacros[ctx->identifier()->getText()];
        //removedMacros.emplace(macro.identifier);
        auto argList = getArgumentList(ctx->argumentList());
        std::vector <std::string> expandedArgs;
        expandedArgs.reserve(argList.size());
        for (const std::string& arg : argList) {
            std::cerr << "recursively going to " << arg << '\n';
            expandedArgs.emplace_back(launchOnce(arg));
        }
        oss << getFunctionBodyReplacement(macro, std::move(expandedArgs));
    } else {
        visitChildren(ctx);
    }
    return {};
}

bool PreprocessorVisitor::isFunctionDefine(antlr4::ParserRuleContext *ctx) const {
    return functionMacros.find(ctx->getText()) != functionMacros.end();
}

std::vector<std::string> PreprocessorVisitor::getArgumentList(Cpp14Parser::ArgumentListContext *ctx) {
    std::vector <std::string> arguments = {ctx->argument()->getText()};
    if (ctx->argumentList()) {
        std::vector<std::string> tail = getArgumentList(ctx->argumentList());
        for (auto& arg : tail) {
            arguments.emplace_back(std::move(arg));
        }
    }
    return arguments;
}

std::string PreprocessorVisitor::getFunctionBodyReplacement(
    const DefineFunctionMacro &macro,
    std::vector<std::string> arguments
) {
    std::ostringstream out;
    for (const auto& token : macro.replacement) {
        if (macro.arguments.find(token) != macro.arguments.end()) {
            size_t num = macro.arguments.at(token);
            out << arguments[num];
        } else {
            out << token;
        }
    }
    return out.str();
}

std::string PreprocessorVisitor::getResult() const {
    return oss.str();
}

bool PreprocessorVisitor::didReplaceFunction() const {
    return replaceFunctionFlag;
}



