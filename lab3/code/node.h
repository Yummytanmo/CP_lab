#ifndef NODE_H
#define NODE_H

// #include <unistd.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define node type
typedef enum _nodeType {
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_ID,
    TOKEN_TYPE,
    // TOKEN_COMMA,
    // TOKEN_SEMI,
    // TOKEN_ASSIGNOP,
    // TOKEN_RELOP,
    // TOKEN_PLUS,
    // TOKEN_MINUS,
    TOKEN_OTHER,
    NOT_A_TOKEN

} NodeType;

typedef enum _kind { BASIC, ARRAY, STRUCTURE, FUNCTION } Kind;
typedef enum _basicType { INT_TYPE, FLOAT_TYPE } BasicType;
typedef enum _errorType {
    UNDEF_VAR = 1,         // 错误类型1：变量在使用时未经定义。
    UNDEF_FUNC,            // 错误类型2：函数在调用时未经定义。
    REDEF_VAR,             // 错误类型3：变量出现重复定义或变量与前面定义过的结构体名字重复。
    REDEF_FUNC,            // 错误类型4：函数出现重复定义（即同样的函数名被多次定义）。
    TYPE_MISMATCH_ASSIGN,  // 错误类型5：赋值号两边的表达式类型不匹配。
    LEFT_VAR_ASSIGN,       // 错误类型6：赋值号左边出现一个只有右值的表达式。
    TYPE_MISMATCH_OP,      // 错误类型7：操作数类型不匹配或操作数类型与操作符不匹配（例如整型变量与数组变量相加减，或数组（或结构体）变量与数组（或结构体）变量相加减）。
    TYPE_MISMATCH_RETURN,  // 错误类型8：return语句的返回类型与函数定义的返回类型不匹配。
    FUNC_AGRC_MISMATCH,    // 错误类型9：过程调用时实参与形参的数目或类型不匹配。
    NOT_A_ARRAY,           // 错误类型10：对非数组型变量使用数组访问操作符。
    NOT_A_FUNC,            // 错误类型11：对非过程名使用过程调用操作符。
    NOT_A_INT,             // 错误类型12：数组访问操作符“[…]”中出现非整数（例如a[1.5]）。
    ILLEGAL_USE_DOT,       // 错误类型13：对非结构体类型变量使用“.”操作符。
    NONEXISTFIELD,         // 错误类型14：访问结构体中未定义过的域。
    REDEF_FEILD,           // 错误类型15：结构体中域名重复定义（指同一结构体中）。
    DUPLICATED_NAME,       // 错误类型16：结构体的名字与前面定义过的结构体或变量的名字重复。
    UNDEF_STRUCT           // 错误类型17：直接使用未定义过的结构体来定义变量。
} ErrorType;


// #define NAME_LENGTH 32
// #define VAL_LENGTH 64

#define TRUE 1
#define FALSE 0

// typedef uint_32 bool;

// node type declared
typedef struct node {
    int lineNo;  //  node in which line
    //   int depth;   //  node depth, for count white space for print
    NodeType type;  // node type
    char* name;     //  node name
    char* val;      //  node value

    struct node* child;  //  non-terminals node first child node
    struct node* next;   //  non-terminals node next brother node

} Node;

typedef unsigned boolean;
typedef Node* pNode;

static inline char* newString(char* src) {
    if (src == NULL) return NULL;
    int length = strlen(src) + 1;
    char* p = (char*)malloc(sizeof(char) * length);
    assert(p != NULL);
    strncpy(p, src, length);
    return p;
}

static inline pNode newNode(int lineNo, NodeType type, char* name, int argc,
                            ...) {
    pNode curNode = NULL;
    // int nameLength = strlen(name) + 1;

    curNode = (pNode)malloc(sizeof(Node));

    assert(curNode != NULL);

    // curNode->name = (char*)malloc(sizeof(char) * NAME_LENGTH);
    // curNode->val = (char*)malloc(sizeof(char) * VAL_LENGTH);
    // curNode->name = (char*)malloc(sizeof(char) * nameLength);
    // assert(curNode->name != NULL);
    // // assert(curNode->val != NULL);

    curNode->lineNo = lineNo;
    curNode->type = type;
    curNode->name = newString(name);
    // strncpy(curNode->name, name, nameLength);

    va_list vaList;
    va_start(vaList, argc);

    pNode tempNode = va_arg(vaList, pNode);

    curNode->child = tempNode;

    for (int i = 1; i < argc; i++) {
        tempNode->next = va_arg(vaList, pNode);
        if (tempNode->next != NULL) {
            tempNode = tempNode->next;
        }
    }

    va_end(vaList);
    return curNode;
}

static inline pNode newTokenNode(int lineNo, NodeType type, char* tokenName,
                                 char* tokenText) {
    pNode tokenNode = (pNode)malloc(sizeof(Node));
    // int nameLength = strlen(tokenName) + 1;
    // int textLength = strlen(tokenText) + 1;

    assert(tokenNode != NULL);

    tokenNode->lineNo = lineNo;
    tokenNode->type = type;

    // tokenNode->name = (char*)malloc(sizeof(char) * nameLength);
    // tokenNode->val = (char*)malloc(sizeof(char) * textLength);

    // assert(tokenNode->name != NULL);
    // assert(tokenNode->val != NULL);

    // strncpy(tokenNode->name, tokenName, nameLength);
    // strncpy(tokenNode->val, tokenText, textLength);

    tokenNode->name = newString(tokenName);
    tokenNode->val = newString(tokenText);

    tokenNode->child = NULL;
    tokenNode->next = NULL;

    return tokenNode;
}

static inline void delNode(pNode* node) {
    if (node == NULL) return;
    pNode p = *node;
    while (p->child != NULL) {
        pNode temp = p->child;
        p->child = p->child->next;
        delNode(&temp);
    }
    free(p->name);
    free(p->val);
    free(p);
    p = NULL;
}

static inline void printTreeInfo(pNode curNode, int height) {
    if (curNode == NULL) {
        return;
    }

    for (int i = 0; i < height; i++) {
        printf("  ");
    }
    printf("%s", curNode->name);
    if (curNode->type == NOT_A_TOKEN) {
        printf(" (%d)", curNode->lineNo);
    } else if (curNode->type == TOKEN_TYPE || curNode->type == TOKEN_ID ||
               curNode->type == TOKEN_INT) {
        printf(": %s", curNode->val);
    } else if (curNode->type == TOKEN_FLOAT) {
        printf(": %lf", atof(curNode->val));
    }
    printf("\n");
    printTreeInfo(curNode->child, height + 1);
    printTreeInfo(curNode->next, height);
}

#endif