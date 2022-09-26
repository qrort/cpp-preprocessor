parser grammar Cpp14Parser;
options {
	tokenVocab = Cpp14Lexer;
}

translationUnit: declarationseq? EOF;
/*Expressions*/

declarationseq: declaration+;

multilineMacro: MultiLineMacro;

directive: Directive;

identifier: Identifier;

functionCall: identifier LeftParen argumentList? RightParen;

argumentList: longSpace? argument (Comma longSpace? argumentList)?;

argument: anyArg | ellipsis;

anyArg: longSpace? anyTokenButComma+ (longSpace? anyArg)?;

anyTokenButComma:
    identifier
    | functionCall
    | CharacterLiteral
    | StringLiteral
    | UserDefinedStringLiteral
    | UserDefinedCharacterLiteral
    | Newline
    | Any
    | bracedSymbol;


anyToken: anyTokenButComma | Comma;

bracedSymbol: LeftParen (bracedSymbol | anyToken*) RightParen;

symbol:
    identifier
    | functionCall
    | CharacterLiteral
    | StringLiteral
    | UserDefinedStringLiteral
    | UserDefinedCharacterLiteral
    | Newline+
    | longSpace
    | Any+;

ppToken:     CharacterLiteral
             | StringLiteral
             | UserDefinedStringLiteral
             | UserDefinedCharacterLiteral
             | Newline
             | Comma
             | longSpace
             | Any
             | LeftParen
             | RightParen;

ellipsis: Ellipsis;

declaration: functionCall
    | multilineMacro
    | directive
    | identifier
    | bracedSymbol
    | ppToken+;

longSpace: Whitespace+;