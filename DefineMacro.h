#ifndef PREPROCESSOR_DEFINEMACRO_H
#define PREPROCESSOR_DEFINEMACRO_H

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

class DefineMacro {
public:
    DefineMacro() = default;
    virtual ~DefineMacro() = default;

    explicit DefineMacro(std::string identifier)
    : identifier(std::move(identifier))
    {
    }

    std::string identifier;
    static size_t counter;
};

class DefineObjectMacro : public DefineMacro {
public:
    DefineObjectMacro() = default;
    DefineObjectMacro(const std::string& identifier, std::string replacement)
    : DefineMacro(identifier), replacement(std::move(replacement))
    {
    }
    std::string replacement;
};

class DefineFunctionMacro : public DefineMacro {
public:
    DefineFunctionMacro() = default;
    DefineFunctionMacro(
            const std::string& identifier,
            std::vector <std::string> replacement,
            const std::vector <std::string>& argList,
            bool isVararg
    )
        : DefineMacro(identifier)
        , replacement(std::move(replacement))
        , isVararg(isVararg)
    {
        arguments.reserve(argList.size());
        for (size_t i = 0; i < argList.size(); i++) {
            arguments.emplace(argList[i], i);
        }
    }
    std::unordered_map <std::string, size_t> arguments;
    std::vector <std::string> replacement;
    bool isVararg;
};


#endif //PREPROCESSOR_DEFINEMACRO_H
