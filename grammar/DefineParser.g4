parser grammar DefineParser;

options {
	tokenVocab = DefineLexer;
}

definesList: controlLine*;

controlLine: objectMacro | functionNoVarargMacro | functionOnlyVarargMacro | functionVarargMacro;

objectMacro: DEFINE longSpace identifier SPACE replacementList NEWLINE;

functionNoVarargMacro: DEFINE longSpace identifier LBRACE identifierList? RBRACE SPACE replacementList NEWLINE;

functionOnlyVarargMacro: DEFINE longSpace identifier LBRACE ELLIPSIS RBRACE SPACE replacementList NEWLINE;

functionVarargMacro: DEFINE longSpace identifier LBRACE identifierList COMMA longSpace ELLIPSIS RBRACE SPACE replacementList NEWLINE;

identifierList: identifier (COMMA longSpace identifierList)?;

replacementList: ppTokens (functionReplacementDelimiter replacementList)?;

ppTokens: ppToken*;

ppToken: LETTER | DIGIT | SPECIAL | UNDERSCORE | DEFINE;

identifier: (LETTER | UNDERSCORE)(LETTER | UNDERSCORE | DIGIT)*;

delimeter: longSpace | COMMA longSpace?;

functionReplacementDelimiter: delimeter | LBRACE | RBRACE;

longSpace: (SPACE | CARET)+;