#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>

#define MAX 10

typedef struct node
{
    struct node* children[MAX];
    struct node* next;
    char name[32];
    int line;
    int num;
    char info[50];
} Tree;

Tree *createNode(char* name, int line, char* info);
int insertNode(Tree *root, Tree *node);
int insertNodes(int num, Tree *root, ...);
void printTree(Tree *root, int level);
void freeTree(Tree *root);
void printNode(Tree *node, int level);
int isSimpleToken(const char *name);
#endif /* TREE_H */
