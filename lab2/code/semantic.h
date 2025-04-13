#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "tree.h"  // 引入语法树相关定义

/* 将语法树结点定义为 pNode */
typedef Tree* pNode;

/* 布尔类型定义 */
typedef int boolean;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* 基本类型 */
typedef enum {
    INT_TYPE,
    FLOAT_TYPE
} BasicType;

/* 类型种类 */
typedef enum {
    BASIC,   // 基本类型
    ARRAY,   // 数组类型
    STRUCT,  // 结构体类型
    FUNC     // 函数类型
} Kind;

/* 前向声明 */
struct SType_;
typedef struct SType_ *pSType;

/* 类型结点 */
typedef struct SType_ {
    Kind kind;      // 类型种类
    union {
        BasicType basicType;  // BASIC 类型
        struct {              // ARRAY 类型
            pSType elemType;
            int length;
        } array;
        struct {              // STRUCT 类型
            char* tag;
            struct FieldNode_* fields;
        } structType;
        struct {              // FUNC 类型
            int argCount;
            struct FieldNode_* argList;
            pSType retType;
        } funcType;
    } u;
} SType, *pSType;

/* 域链表结点 */
typedef struct FieldNode_ {
    char* name;
    pSType type;
    struct FieldNode_* next;
} FieldNode, *pFieldNode;

/* 符号项 */
typedef struct SymbolItem_ {
    int depth;             // 作用域深度
    pFieldNode field;      // 对应的标识符及其类型信息
    struct SymbolItem_* nextInHash;  // 哈希表中冲突链表指针
    struct SymbolItem_* nextInScope; // 作用域链表指针
} SymbolItem, *pSymbolItem;

#define HASH_TABLE_SIZE 16384

/* 符号哈希表 */
typedef struct SymHash_ {
    pSymbolItem *buckets;   // 符号项数组（指针数组）
} SymHash, *pSymHash;

/* 作用域栈（用于管理不同作用域中的符号） */
typedef struct SymStack_ {
    int currentDepth;         // 当前作用域深度
    pSymbolItem *stackArray;  // 每个深度下符号链表的头指针数组
} SymStack, *pSymStack;

/* 符号表 */
typedef struct SymbolTable_ {
    pSymHash hash;         // 符号哈希表
    pSymStack scope;       // 作用域管理栈
    int anonStructCount;   // 匿名结构体计数器
} SymbolTable, *pSymbolTable;

extern pSymbolTable symbolTable; // 声明外部变量
/* --- 函数原型 --- */

/* 辅助函数 */
unsigned getHashCode(char* str);
char* newString(const char* s);

/* 错误代码定义 */
#define UNDEF_VAR         1  // 错误类型1：变量在使用时未经定义
#define UNDEF_FUNC        2  // 错误类型2：函数在调用时未经定义
#define REDEF_VAR         3  // 错误类型3：变量重复定义，或变量与之前定义的结构体名冲突
#define REDEF_FUNC        4  // 错误类型4：函数重复定义
#define TYPE_MISMATCH_ASSIGN 5  // 错误类型5：赋值两侧表达式类型不匹配
#define LEFT_VAR_ASSIGN   6  // 错误类型6：赋值号左边为非左值表达式
#define TYPE_MISMATCH_OP  7  // 错误类型7：操作数类型不匹配或操作符使用错误
#define TYPE_MISMATCH_RETURN 8  // 错误类型8：return 语句返回值与函数定义不匹配
#define FUNC_ARG_MISMATCH 9  // 错误类型9：函数调用时实参与形参数目或类型不匹配
#define NOT_A_ARRAY       10 // 错误类型10：对非数组变量使用数组下标“[…]”操作符
#define NOT_A_FUNC        11 // 错误类型11：对普通变量使用函数调用操作符“(…)”
#define NON_INT_INDEX     12 // 错误类型12：数组下标不是整数
#define NOT_A_STRUCT      13 // 错误类型13：对非结构体变量使用“.”操作符
#define NONEXIST_FIELD    14 // 错误类型14：访问结构体中未定义的域
#define REDEF_FIELD       15 // 错误类型15：结构体内字段重复定义或对字段进行初始化
#define REDEF_STRUCT      16 // 错误类型16：结构体名字与之前定义冲突
#define UNDEF_STRUCT_TYPE 17 // 错误类型17：使用未定义的结构体类型

void pError(int errorCode, int line, char* msg);

/* 类型相关函数 */
pSType createSType(Kind kind, ...);
pSType cloneSType(pSType orig);
void freeSType(pSType typeNode);
boolean checkSTypeEqual(pSType t1, pSType t2);
void displaySType(pSType typeNode);

/* 域链表相关函数 */
pFieldNode createFieldNode(char* fieldName, pSType fieldType);
pFieldNode cloneFieldNode(pFieldNode src);
void freeFieldNode(pFieldNode node);
void updateFieldName(pFieldNode node, char* newName);
void displayFieldNode(pFieldNode node);

/* 符号项与符号表相关函数 */
pSymbolItem createSymbolItem(int depth, pFieldNode field);
void freeSymbolItem(pSymbolItem item);
pSymHash createSymHash(void);
void freeSymHash(pSymHash hashTbl);
pSymbolTable initSymTable(void);
void freeSymTable(pSymbolTable symTab);
void addSymbolItem(pSymbolTable symTab, pSymbolItem item);
pSymbolItem lookupSymbol(pSymbolTable symTab, char* name);

/* 作用域栈函数（请根据实际项目实现或替换） */
pSymStack initScopeStack(void);
void freeScopeStack(pSymStack stack);
pSymbolItem getScopeHead(pSymStack stack);
void setScopeHead(pSymStack stack, pSymbolItem head);
void addScopeDepth(pSymStack stack);
void decreaseScopeDepth(pSymStack stack);

/* 语义分析函数 */
void processTree(pNode root);
void processExtDef(pNode node);
void processExtDecList(pNode node, pSType specType);
pSType processSpecifier(pNode node);
pSType processStructSpecifier(pNode node);
pSymbolItem processVarDec(pNode node, pSType specType);
void processFunDec(pNode node, pSType retType);
void processVarList(pNode node, pSymbolItem funcSym);
pFieldNode processParamDec(pNode node);
void processCompSt(pNode node, pSType retType);
void processStmtList(pNode node, pSType retType);
void processStmt(pNode node, pSType retType);
void processDefList(pNode node, pSymbolItem structSym);
void processDef(pNode node, pSymbolItem structSym);
void processDecList(pNode node, pSType specType, pSymbolItem structSym);
void processDec(pNode node, pSType specType, pSymbolItem structSym);
pSType processExp(pNode node);
void processArgs(pNode node, pSymbolItem funcSym);

/* 判断结构体是否已经定义 */
boolean isStructDefined(pSymbolItem item);

#endif  /* SEMANTIC_H */
