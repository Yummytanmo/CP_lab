#include <errno.h>
#include "inter.h"
#include "syntax.tab.h"

extern pNode root;

extern int yylineno;
extern int yyparse();
extern void yyrestart(FILE*);

unsigned lexError = FALSE;
unsigned synError = FALSE;

int main(int argc, char** argv) {
    if (argc <= 1) {
        return 1;
    }

    FILE* fr = fopen(argv[1], "r");
    if (!fr) {
        return 1;
    }

    yyrestart(fr);
    yyparse();

    if (!lexError && !synError) {
        table = initTable();
        traverseTree(root);
        interCodeList = newInterCodeList();
        genInterCodes(root);
        if (!interError) {
            printInterCode(NULL, interCodeList); // Print to console
        }
        deleteTable(table);
    }

    delNode(&root);
    return 0;
}