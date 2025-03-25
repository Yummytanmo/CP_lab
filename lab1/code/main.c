#include <stdio.h>
#include "tree.h"

Tree *root = NULL;
int error_num = 0;

extern FILE* yyin;
extern int yylex();
extern int yyrestart(FILE *);
extern int yyparse();
extern int yydebug;

int main(int argc,char** argv){
    if(argc>1){
      if(!(yyin=fopen(argv[1],"r"))){
      perror(argv[1]);
      return 1;
      }
    }
      // yydebug = 1;
      yyrestart(yyin);
      yyparse();
      if (error_num == 0)
          printTree(root, 0);
      freeTree(root);
      return 0;
}
