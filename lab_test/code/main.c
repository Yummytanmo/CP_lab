#include <stdio.h>
#include "tree.h"
#include "semantic.h"  // 包含语义分析头文件

Tree *root = NULL;
int error_num = 0;          // 语法/词法错误计数器
int semantic_error_num = 0; // 语义错误计数器

extern FILE* yyin;
extern int yylex();
extern int yyrestart(FILE *);
extern int yyparse();

int main(int argc, char** argv) {
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    
    yyrestart(yyin);
    yyparse();  // 执行语法分析，生成语法树
    
    // 只有语法/词法无错误时才进行语义分析
    if (error_num == 0) {
        symbolTable = initSymTable();   // 初始化符号表
        processTree(root);              // 执行语义分析
        freeSymTable(symbolTable);      // 释放符号表资源
    }
    freeTree(root);
    return 0;
}