#include "tree.h"

Tree *createNode(char* name, int line, char* info)
{
    Tree *node = (Tree *)malloc(sizeof(Tree));
    if (node == NULL)
    {
        fprintf(stderr, "Out of memory. Aborting...\n");
        exit(1);
    }
    for (int i = 0; i < MAX; i++)
    {
        node->children[i] = NULL;
    }
    strncpy(node->name, name, 32);
    node->line = line;
    node->num = 0;
    node->next = NULL;
    strncpy(node->info, info, 50);
    return node;
}

int insertNode(Tree *root, Tree *node)
{
    if (root->num < MAX)
    {
        root->children[root->num] = node;
        if (root->num > 0)
        {
            root->children[root->num - 1]->next = node;
        }
        root->num++;

        return 1;
    }
    else
    {
        fprintf(stderr, "Node is full. Aborting...\n");
        exit(1);
    }
}

int insertNodes(int num, Tree *root, ...)
{
    va_list args;
    va_start(args, root);
    for (int i = 0; i < num; i++)
    {
        Tree *node = va_arg(args, Tree *);
        if (insertNode(root, node) == 0)
        {
            return 0;
        }
    }
    va_end(args);
    return 1;
}


void printTree(Tree *root, int level){
    if(root == NULL){
        return;
    }
    printNode(root, level);
    for(int i = 0; i < root->num; i++){
        printTree(root->children[i], level + 1);
    }
}

void printNode(Tree *node, int level){
    for(int i = 0; i < level; i++){
        printf("  ");
    }
    
    if (strcmp(node->name, "INT") == 0) {
        printf("%s: %d\n", node->name, atoi(node->info));
    } else if (strcmp(node->name, "FLOAT") == 0) {
        printf("%s: %f\n", node->name, atof(node->info));
    } else if (strcmp(node->name, "TYPE") == 0 || strcmp(node->name, "ID") == 0) {
        printf("%s: %s\n", node->name, node->info);
    } else if (isSimpleToken(node->name)) {
        printf("%s\n", node->name);
    } else {
        printf("%s (%d)\n", node->name, node->line);
    }

}

int isSimpleToken(const char *name) {
    const char *tokens[] = {
        "SEMI", "COMMA", "ASSIGNOP", "RELOP", "PLUS", "MINUS", "STAR", "DIV",
        "AND", "OR", "DOT", "NOT", "LP", "RP", "LB", "RB", "LC", "RC",
        "STRUCT", "RETURN", "IF", "ELSE", "WHILE"
    };
    int size = sizeof(tokens) / sizeof(tokens[0]);
    for (int i = 0; i < size; i++) {
        if (strcmp(name, tokens[i]) == 0)
            return 1;
    }
    return 0;
}

void freeTree(Tree *root){
    if(root == NULL){
        return;
    }
    for(int i = 0; i < root->num; i++){
        freeTree(root->children[i]);
    }
    free(root);
}