
%option yylineno
yylineno = 1

%{
    
%}

ws          [ \t]
digit       [0-9]
letter      [_a-zA-Z]
ld          [0-9_a-zA-Z]
nonletter   [^_a-zA-Z]
RELOP       >|<|>=|<=|==|!=
IF          if
ELSE        else
WHILE       while
TYPE        int|float
STRUCT      struct
RETURN      return
PLUS        "+"
MINUS       "-"
STAR        "*"
DIV         "/"
AND         "&&"
OR          "||"
NOT         "!"
DOT         "."
SEMI        ";"
COMMA       ","
ASSIGNOP    "="
LP          "("
RP          ")"
LB          "["
RB          "]"
LC          "{"
RC          "}"
ID          {letter}{ld}*
NONZERO     [1-9]
OCTDIGIT    [0-7]
HEXDIGIT    [0-9a-fA-F]
INT         0[xX]{HEXDIGIT}+|0{OCTDIGIT}+|{NONZERO}{digit}*|0
FLOAT       {digit}+"."{digit}+|{digit}*"."{digit}+[eE][+-]?{digit}+|{digit}+"."{digit}*[eE][+-]?{digit}+

%%
\n          { yylineno; }
{ws}        {/* 忽略空白字符 */}
{IF}        { printf("Line %d: IF (%s)\n", yylineno, yytext); }
{ELSE}      { printf("Line %d: ELSE (%s)\n", yylineno, yytext); }
{WHILE}     { printf("Line %d: WHILE (%s)\n", yylineno, yytext); }
{TYPE}      { printf("Line %d: TYPE (%s)\n", yylineno, yytext); }
{STRUCT}    { printf("Line %d: STRUCT (%s)\n", yylineno, yytext); }
{RETURN}    { printf("Line %d: RETURN (%s)\n", yylineno, yytext); }
{RELOP}     { printf("Line %d: RELOP (%s)\n", yylineno, yytext); }
{PLUS}      { printf("Line %d: PLUS (%s)\n", yylineno, yytext); }
{MINUS}     { printf("Line %d: MINUS (%s)\n", yylineno, yytext); }
{STAR}      { printf("Line %d: STAR (%s)\n", yylineno, yytext); }
{DIV}       { printf("Line %d: DIV (%s)\n", yylineno, yytext); }
{AND}       { printf("Line %d: AND (%s)\n", yylineno, yytext); }
{OR}        { printf("Line %d: OR (%s)\n", yylineno, yytext); }
{NOT}       { printf("Line %d: NOT (%s)\n", yylineno, yytext); }
{DOT}       { printf("Line %d: DOT (%s)\n", yylineno, yytext); }
{SEMI}      { printf("Line %d: SEMI (%s)\n", yylineno, yytext); }
{COMMA}     { printf("Line %d: COMMA (%s)\n", yylineno, yytext); }
{ASSIGNOP}  { printf("Line %d: ASSIGNOP (%s)\n", yylineno, yytext); }
{LP}        { printf("Line %d: LP (%s)\n", yylineno, yytext); }
{RP}        { printf("Line %d: RP (%s)\n", yylineno, yytext); }
{LB}        { printf("Line %d: LB (%s)\n", yylineno, yytext); }
{RB}        { printf("Line %d: RB (%s)\n", yylineno, yytext); }
{LC}        { printf("Line %d: LC (%s)\n", yylineno, yytext); }
{RC}        { printf("Line %d: RC (%s)\n", yylineno, yytext); }
{ID}        { printf("Line %d: ID (%s)\n", yylineno, yytext); }
{INT}       { printf("Line %d: INT (%s)\n", yylineno, yytext); }
{FLOAT}     { printf("Line %d: FLOAT (%s)\n", yylineno, yytext); }

.           { printf("Error type A at Line %d: Mysterious character '%s'\n", yylineno, yytext); }

%%
