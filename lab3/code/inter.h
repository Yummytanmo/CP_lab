#ifndef INTER_H
#define INTER_H
#include "node.h"
#include "semantic.h"

typedef struct _operand* pOperand;
typedef struct _interCode* pInterCode;
typedef struct _interCodes* pInterCodes;
typedef struct _arg* pArg;
typedef struct _argList* pArgList;
typedef struct _interCodeList* pInterCodeList;

typedef struct _operand {
    enum {
        OP_VARIABLE,
        OP_CONSTANT,
        OP_ADDRESS,
        OP_LABEL,
        OP_FUNCTION,
        OP_RELOP,
    } kind;

    union {
        int value;
        char* name;
    } u;

    // boolean isAddr;
} Operand;

typedef struct _interCode {
    enum {
        IR_LABEL,      // 标签指令：标记基本块的起始位置
        IR_FUNCTION,   // 函数定义指令：标记函数入口，定义函数名
        IR_ASSIGN,     // 赋值指令：实现变量与变量或常量间的赋值操作
        IR_ADD,        // 加法运算指令：实现两个操作数的加法
        IR_SUB,        // 减法运算指令：实现两个操作数的减法
        IR_MUL,        // 乘法运算指令：实现两个操作数的乘法
        IR_DIV,        // 除法运算指令：实现两个操作数的除法
        IR_GET_ADDR,   // 取地址指令：获取变量的地址，例如 t := &a
        IR_READ_ADDR,  // 读取地址指令：从地址中读出值，例如 t := *a
        IR_WRITE_ADDR, // 写入地址指令：将值写入指定地址，例如 *a := b
        IR_GOTO,       // 无条件跳转指令：程序流程的跳转
        IR_IF_GOTO,    // 条件跳转指令：根据条件判断进行跳转
        IR_RETURN,     // 返回指令：函数返回数据或结束函数
        IR_DEC,        // 内存分配指令：为变量，通常是数组或结构体分配内存空间
        IR_ARG,        // 传递参数指令：为函数调用传递实际参数
        IR_CALL,       // 函数调用指令：调用其他函数
        IR_PARAM,      // 形式参数指令：标记函数定义中的参数
        IR_READ,       // 读指令：从标准输入读取数据
        IR_WRITE,      // 写指令：将数据输出到标准输出
    } kind;

    union {
        struct {
            pOperand op;
        } oneOp;
        struct {
            pOperand right, left;
        } assign;
        struct {
            pOperand result, op1, op2;
        } binOp;
        struct {
            pOperand x, relop, y, z;
        } ifGoto;
        struct {
            pOperand op;
            int size;
        } dec;
    } u;
} InterCode;

typedef struct _interCodes {
    pInterCode code;
    pInterCodes *prev, *next;
} InterCodes;

typedef struct _arg {
    pOperand op;
    pArg next;
} Arg;

typedef struct _argList {
    pArg head;
    pArg cur;
} ArgList;

typedef struct _interCodeList {
    pInterCodes head;
    pInterCodes cur;
    char* lastArrayName;  // 针对结构体数组，因为需要数组名查表
    int tempVarNum;
    int labelNum;
} InterCodeList;

extern boolean interError;
extern pInterCodeList interCodeList;

// Operand func
pOperand newOperand(int kind, ...);
void deleteOperand(pOperand p);
void setOperand(pOperand p, int kind, void* val);
void printOp(FILE* fp, pOperand op);

// InterCode func
pInterCode newInterCode(int kind, ...);
void deleteInterCode(pInterCode p);
void printInterCode(FILE* fp, pInterCodeList interCodeList);

// InterCodes func
pInterCodes newInterCodes(pInterCode code);
void deleteInterCodes(pInterCodes p);

// Arg and ArgList func
pArg newArg(pOperand op);
pArgList newArgList();
void deleteArg(pArg p);
void deleteArgList(pArgList p);
void addArg(pArgList argList, pArg arg);

// InterCodeList func
pInterCodeList newInterCodeList();
void deleteInterCodeList(pInterCodeList p);
void addInterCode(pInterCodeList interCodeList, pInterCodes newCode);

// traverse func
pOperand newTemp();
pOperand newLabel();
int getSize(pType type);
void genInterCodes(pNode node);
void genInterCode(int kind, ...);
void translateExp(pNode node, pOperand place);
void translateArgs(pNode node, pArgList argList);
void translateCond(pNode node, pOperand labelTrue, pOperand labelFalse);
void translateVarDec(pNode node, pOperand place);
void translateDec(pNode node);
void translateDecList(pNode node);
void translateDef(pNode node);
void translateDefList(pNode node);
void translateCompSt(pNode node);
void translateStmt(pNode node);
void translateStmtList(pNode node);
void translateFunDec(pNode node);
void translateExtDef(pNode node);
void translateExtDefList(pNode node);
#endif