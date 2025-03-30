
%option yylineno

%{
    #include "tree.h"
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    #include "syntax.tab.h"
    extern int error_num;
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
OCT         0[0-7]+
HEX         0[xX][0-9A-Fa-f]+
INT         0|([1-9][0-9]*)
FLOAT       {digit}+"."{digit}+|{digit}*"."{digit}+[eE][+-]?{digit}+|{digit}+"."{digit}*[eE][+-]?{digit}+

%%
\n { yycolumn = 1; }
{ws} { /* 忽略空白字符 */ }
{IF} { yylval.node = createNode("IF", yylineno, ""); return IF; }
{ELSE} { yylval.node = createNode("ELSE", yylineno, ""); return ELSE; }
{WHILE} { yylval.node = createNode("WHILE", yylineno, ""); return WHILE; }
{TYPE} { yylval.node = createNode("TYPE", yylineno, yytext); return TYPE; }
{STRUCT} { yylval.node = createNode("STRUCT", yylineno, ""); return STRUCT; }
{RETURN} { yylval.node = createNode("RETURN", yylineno, ""); return RETURN; }
{RELOP} { yylval.node = createNode("RELOP", yylineno, ""); return RELOP; }
{PLUS} { yylval.node = createNode("PLUS", yylineno, ""); return PLUS; }
{MINUS} { yylval.node = createNode("MINUS", yylineno, ""); return MINUS; }
{STAR} { yylval.node = createNode("STAR", yylineno, ""); return STAR; }
{DIV} { yylval.node = createNode("DIV", yylineno, ""); return DIV; }
{AND} { yylval.node = createNode("AND", yylineno, ""); return AND; }
{OR} { yylval.node = createNode("OR", yylineno, ""); return OR; }
{NOT} { yylval.node = createNode("NOT", yylineno, ""); return NOT; }
{DOT} { yylval.node = createNode("DOT", yylineno, ""); return DOT; }
{SEMI} { yylval.node = createNode("SEMI", yylineno, ""); return SEMI; }
{COMMA} { yylval.node = createNode("COMMA", yylineno, ""); return COMMA; }
{ASSIGNOP} { yylval.node = createNode("ASSIGNOP", yylineno, ""); return ASSIGNOP; }
{LP} { yylval.node = createNode("LP", yylineno, ""); return LP; }
{RP} { yylval.node = createNode("RP", yylineno, ""); return RP; }
{LB} { yylval.node = createNode("LB", yylineno, ""); return LB; }
{RB} { yylval.node = createNode("RB", yylineno, ""); return RB; }
{LC} { yylval.node = createNode("LC", yylineno, ""); return LC; }
{RC} { yylval.node = createNode("RC", yylineno, ""); return RC; }
{ID} { yylval.node = createNode("ID", yylineno, yytext); return ID; }
{INT} { yylval.node = createNode("INT", yylineno, yytext); return INT; }
{FLOAT} {
    double value = strtod(yytext, NULL);
    yylval.node = createNode("FLOAT", yylineno, value);
    return FLOAT;
}
{OCT} {
    char val[32];
    int tmp = strtol(yytext, NULL, 8);
    sprintf(val, "%d", tmp);
    yylval.node = createNode("INT", yylineno, val);
    return INT;
}
{HEX} {
    char val[32];
    int tmp = strtol(yytext, NULL, 16);
    sprintf(val, "%d", tmp);
    yylval.node = createNode("INT", yylineno, val);
    return INT;
}


. { error_num++; fprintf(stderr, "Error type A at Line %d: Mysterious character '%s'\n", yylineno, yytext); }

%%
