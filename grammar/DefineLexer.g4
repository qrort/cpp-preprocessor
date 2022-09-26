lexer grammar DefineLexer;

DEFINE: '#define';

LBRACE: '(';

RBRACE: ')';

COMMA: ',';

NEWLINE: '\n';

ELLIPSIS: '...';

LETTER: [a-zA-Z];

UNDERSCORE: '_';

DIGIT: [0-9];

SPECIAL: ~[A-Za-z0-9(),_ \n\t\r];

SPACE: ' ';

CARET: ('\t' | '\r');