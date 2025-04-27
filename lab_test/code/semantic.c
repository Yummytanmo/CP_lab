#include "semantic.h"

#define child(n) ((n)->children[0])
#define next(n) ((n)->next)

pSymbolTable symbolTable = NULL;

void pError(int errorCode, int line, char* msg){    
    printf("Error type %d at Line %d: %s\n", errorCode, line, msg);
}

// *********************** 类型相关函数 ***********************

pSType createSType(Kind kind, ...) {
    pSType typeNode = (pSType)malloc(sizeof(SType));
    if (typeNode == NULL) {
        fprintf(stderr, "Out of memory in createSType. Aborting...\n");
        exit(1);
    }
    typeNode->kind = kind;
    va_list args;
    assert(kind == BASIC || kind == ARRAY || kind == STRUCT || kind == FUNC);
    switch (kind) {
        case BASIC:
            va_start(args, kind);
            typeNode->u.basicType = va_arg(args, BasicType);
            va_end(args);
            break;
        case ARRAY:
            va_start(args, kind);
            typeNode->u.array.elemType = va_arg(args, pSType);
            typeNode->u.array.length = va_arg(args, int);
            va_end(args);
            break;
        case STRUCT:
            va_start(args, kind);
            typeNode->u.structType.tag = va_arg(args, char*);
            typeNode->u.structType.fields = va_arg(args, pFieldNode);
            va_end(args);
            break;
        case FUNC:
            va_start(args, kind);
            typeNode->u.funcType.argCount = va_arg(args, int);
            typeNode->u.funcType.argList = va_arg(args, pFieldNode);
            typeNode->u.funcType.retType = va_arg(args, pSType);
            va_end(args);
            break;
    }
    return typeNode;
}

pSType cloneSType(pSType orig) {
    if (orig == NULL) return NULL;
    pSType newTypeNode = (pSType)malloc(sizeof(SType));
    assert(newTypeNode != NULL);
    newTypeNode->kind = orig->kind;
    switch (orig->kind) {
        case BASIC:
            newTypeNode->u.basicType = orig->u.basicType;
            break;
        case ARRAY:
            newTypeNode->u.array.elemType = cloneSType(orig->u.array.elemType);
            newTypeNode->u.array.length = orig->u.array.length;
            break;
        case STRUCT:
            newTypeNode->u.structType.tag = newString(orig->u.structType.tag);
            newTypeNode->u.structType.fields = cloneFieldNode(orig->u.structType.fields);
            break;
        case FUNC:
            newTypeNode->u.funcType.argCount = orig->u.funcType.argCount;
            newTypeNode->u.funcType.argList = cloneFieldNode(orig->u.funcType.argList);
            newTypeNode->u.funcType.retType = cloneSType(orig->u.funcType.retType);
            break;
    }
    return newTypeNode;
}

void freeSType(pSType typeNode) {
    assert(typeNode != NULL);
    switch (typeNode->kind) {
        case BASIC:
            break;
        case ARRAY:
            freeSType(typeNode->u.array.elemType);
            typeNode->u.array.elemType = NULL;
            break;
        case STRUCT: {
            if (typeNode->u.structType.tag)
                free(typeNode->u.structType.tag);
            typeNode->u.structType.tag = NULL;
            pFieldNode cur = typeNode->u.structType.fields;
            while (cur) {
                pFieldNode next = cur->next;
                freeFieldNode(cur);
                cur = next;
            }
            typeNode->u.structType.fields = NULL;
            break;
        }
        case FUNC: {
            freeSType(typeNode->u.funcType.retType);
            typeNode->u.funcType.retType = NULL;
            pFieldNode cur = typeNode->u.funcType.argList;
            while (cur) {
                pFieldNode next = cur->next;
                freeFieldNode(cur);
                cur = next;
            }
            typeNode->u.funcType.argList = NULL;
            break;
        }
    }
    free(typeNode);
}

boolean checkSTypeEqual(pSType t1, pSType t2) {
    if (t1 == NULL || t2 == NULL) return TRUE;
    if (t1->kind == FUNC || t2->kind == FUNC) return FALSE;
    if (t1->kind != t2->kind) return FALSE;
    switch (t1->kind) {
        case BASIC:
            return t1->u.basicType == t2->u.basicType;
        case ARRAY:
            return checkSTypeEqual(t1->u.array.elemType, t2->u.array.elemType);
        case STRUCT:
            return strcmp(t1->u.structType.tag, t2->u.structType.tag) == 0;
        default:
            return FALSE;
    }
}

void displaySType(pSType typeNode) {
    if (typeNode == NULL) {
        printf("Type node is NULL.\n");
        return;
    }
    printf("Type kind: %d\n", typeNode->kind);
    switch (typeNode->kind) {
        case BASIC:
            printf("Basic type: %d\n", typeNode->u.basicType);
            break;
        case ARRAY:
            printf("Array length: %d\n", typeNode->u.array.length);
            displaySType(typeNode->u.array.elemType);
            break;
        case STRUCT:
            if (typeNode->u.structType.tag)
                printf("Struct tag: %s\n", typeNode->u.structType.tag);
            else
                printf("Anonymous struct.\n");
            displayFieldNode(typeNode->u.structType.fields);
            break;
        case FUNC:
            printf("Function arg count: %d\n", typeNode->u.funcType.argCount);
            displayFieldNode(typeNode->u.funcType.argList);
            printf("Function return type:\n");
            displaySType(typeNode->u.funcType.retType);
            break;
    }
}

// *********************** 域链表相关函数 ***********************

pFieldNode createFieldNode(char* fieldName, pSType fieldType) {
    pFieldNode newNode = (pFieldNode)malloc(sizeof(FieldNode));
    assert(newNode != NULL);
    newNode->name = newString(fieldName);
    newNode->type = fieldType;
    newNode->next = NULL;
    return newNode;
}

pFieldNode cloneFieldNode(pFieldNode src) {
    if (!src) return NULL;
    pFieldNode head = NULL, cur = NULL;
    pFieldNode tmp = src;
    while (tmp) {
        pFieldNode newNode = createFieldNode(tmp->name, cloneSType(tmp->type));
        if (head == NULL) {
            head = newNode;
            cur = head;
        } else {
            cur->next = newNode;
            cur = newNode;
        }
        tmp = tmp->next;
    }
    return head;
}

void freeFieldNode(pFieldNode node) {
    if (node == NULL) return;
    if (node->name) {
        free(node->name);
        node->name = NULL;
    }
    if (node->type) freeSType(node->type);
    free(node);
}

void updateFieldName(pFieldNode node, char* newName) {
    assert(node && newName);
    if (node->name)
        free(node->name);
    node->name = newString(newName);
}

void displayFieldNode(pFieldNode node) {
    if (node == NULL) {
        printf("Field list is empty.\n");
        return;
    }
    printf("Field name: %s\n", node->name);
    printf("Field type:\n");
    displaySType(node->type);
    displayFieldNode(node->next);
}

// *********************** 符号项与符号表相关函数 ***********************

pSymbolItem createSymbolItem(int depth, pFieldNode field) {
    pSymbolItem symItem = (pSymbolItem)malloc(sizeof(SymbolItem));
    assert(symItem != NULL);
    symItem->depth = depth;
    symItem->field = field;
    symItem->nextInHash = NULL;
    symItem->nextInScope = NULL;
    return symItem;
}

void freeSymbolItem(pSymbolItem item) {
    assert(item != NULL);
    if (item->field != NULL) {
        freeFieldNode(item->field);
    }
    free(item);
}

pSymHash createSymHash() {
    pSymHash hashTbl = (pSymHash)malloc(sizeof(SymHash));
    assert(hashTbl != NULL);
    hashTbl->buckets = (pSymbolItem*)malloc(sizeof(pSymbolItem) * HASH_TABLE_SIZE);
    assert(hashTbl->buckets != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hashTbl->buckets[i] = NULL;
    }
    return hashTbl;
}

void freeSymHash(pSymHash hashTbl) {
    assert(hashTbl != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        pSymbolItem cur = hashTbl->buckets[i];
        while (cur) {
            pSymbolItem next = cur->nextInHash;
            freeSymbolItem(cur);
            cur = next;
        }
        hashTbl->buckets[i] = NULL;
    }
    free(hashTbl->buckets);
    free(hashTbl);
}

void addSymbolItem(pSymbolTable symTab, pSymbolItem item) {
    assert(symTab != NULL && item != NULL);
    unsigned hashVal = getHashCode(item->field->name);
    item->nextInScope = getScopeHead(symTab->scope);
    setScopeHead(symTab->scope, item);
    item->nextInHash = symTab->hash->buckets[hashVal];
    symTab->hash->buckets[hashVal] = item;
}

void removeScopeSymbols(pSymbolTable symTab) {
    assert(symTab != NULL);
    pSymStack scope = symTab->scope;
    pSymbolItem cur = getScopeHead(scope);
    while (cur) {
        unsigned hashVal = getHashCode(cur->field->name);
        if (cur == symTab->hash->buckets[hashVal])
            symTab->hash->buckets[hashVal] = cur->nextInHash;
        else {
            pSymbolItem tmp = symTab->hash->buckets[hashVal];
            while (tmp && tmp != cur) {
                tmp = tmp->nextInHash;
            }
            if (tmp)
                tmp->nextInHash = cur->nextInHash;
        }
        pSymbolItem delItem = cur;
        cur = delItem->nextInScope;  // 注意使用链表的 nextInScope（原代码使用 next）
        freeSymbolItem(delItem);
    }
    setScopeHead(scope, NULL);
    decreaseScopeDepth(scope);
}

pSymbolItem lookupSymbol(pSymbolTable symTab, char* name) {
    unsigned hashVal = getHashCode(name);
    pSymbolItem cur = symTab->hash->buckets[hashVal];
    while (cur) {
        if (strcmp(cur->field->name, name) == 0)
            return cur;
        cur = cur->nextInHash;
    }
    return NULL;
}

// *********************** 符号表初始化及销毁 ***********************

pSymbolTable initSymTable() {
    pSymbolTable symTab = (pSymbolTable)malloc(sizeof(SymbolTable));
    assert(symTab != NULL);
    symTab->hash = createSymHash();
    symTab->scope = initScopeStack();
    symTab->anonStructCount = 0;
    return symTab;
}

void freeSymTable(pSymbolTable symTab) {
    freeSymHash(symTab->hash);
    symTab->hash = NULL;
    freeScopeStack(symTab->scope);
    symTab->scope = NULL;
    free(symTab);
}

// *********************** 语义分析各部分函数 ***********************

void processTree(pNode root) {
    if (root == NULL) return;
    if (strcmp(root->name, "ExtDef") == 0) processExtDef(root);
    processTree(child(root));
    processTree(next(root));
}

/*
ExtDef → Specifier ExtDecList SEMI
| Specifier SEMI
| Specifier FunDec CompSt
*/
void processExtDef(pNode node) {
    assert(node != NULL);
    pSType specType = processSpecifier(child(node));
    char* secondToken = next(child(node))->name;
    if (strcmp(secondToken, "ExtDecList") == 0) {
        processExtDecList(next((node)), specType);
    } else if (strcmp(secondToken, "FunDec") == 0) {
        processFunDec(next(child(node)), specType);
        processCompSt(next(next(child(node))), specType);
    }
    if (specType) freeSType(specType);
}

/*
ExtDecList → VarDec
| VarDec COMMA ExtDecList
*/
void processExtDecList(pNode node, pSType specType) {
    assert(node != NULL);
    pNode cur = node;
    while (cur) {
        pSymbolItem newSym = processVarDec(child(cur), specType);
        if (lookupSymbol(symbolTable, newSym->field->name) != NULL) {
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", newSym->field->name);
            pError(REDEF_VAR, cur->line, msg);
            freeSymbolItem(newSym);
        } else {
            addSymbolItem(symbolTable, newSym);
        }
        if (next(next((child(cur)))))
            cur = next(next((child(cur))));
        else
            break;
    }
}

/*
Specifier → TYPE
| StructSpecifier
*/
pSType processSpecifier(pNode node) {
    assert(node != NULL);
    pNode sub = child(node);
    if (strcmp(sub->name, "TYPE") == 0) {
        if (strcmp(sub->info, "float") == 0)
            return createSType(BASIC, FLOAT_TYPE);
        else
            return createSType(BASIC, INT_TYPE);
    }
    //  else {
    //     return processStructSpecifier(sub);
    // }
}

pSType processStructSpecifier(pNode node) {
    assert(node != NULL);
    pSType retType = NULL;
    pNode tagNode = child(next(node));
    if (strcmp(tagNode->name, "Tag") ) {
        pSymbolItem structSym = createSymbolItem(symbolTable->scope->currentDepth, 
                                  createFieldNode("", createSType(STRUCT, NULL, NULL)));
        if (strcmp(tagNode->name, "OptTag") == 0) {
            updateFieldName(structSym->field, child(tagNode)->info);
            tagNode = next(tagNode);
        } else {
            symbolTable->anonStructCount++;
            char tmpName[20] = {0};
            sprintf(tmpName, "%d", symbolTable->anonStructCount);
            updateFieldName(structSym->field, tmpName);
        }
        if (strcmp(next(tagNode)->name, "DefList") == 0) {
            processDefList(next(tagNode), structSym);
        }
        if (lookupSymbol(symbolTable, structSym->field->name) != NULL) {
            char msg[100] = {0};
            sprintf(msg, "Duplicated name \"%s\".", structSym->field->name);
            pError(REDEF_STRUCT, node->line, msg);
            freeSymbolItem(structSym);
        } else {
            retType = createSType(STRUCT, newString(structSym->field->name),
                          cloneFieldNode(structSym->field->type->u.structType.fields));
            if (strcmp(next(node)->name, "OptTag") == 0)
                addSymbolItem(symbolTable, structSym);
            else
                freeSymbolItem(structSym);
        }
    } else {
        pSymbolItem existStruct = lookupSymbol(symbolTable, child(tagNode)->info);
        if (existStruct == NULL || !isStructDefined(existStruct)) {
            char msg[100] = {0};
            sprintf(msg, "Undefined structure \"%s\".", child(tagNode)->info);
            pError(UNDEF_STRUCT_TYPE, node->line, msg);
        } else {
            retType = createSType(STRUCT, newString(existStruct->field->name),
                        cloneFieldNode(existStruct->field->type->u.structType.fields));
        }
    }
    return retType;
}

/*
VarDec → ID
| VarDec LB INT RB
*/
pSymbolItem processVarDec(pNode node, pSType specType) {
    assert(node != NULL);
    pNode idNode = node;
    while (child(idNode)) {
        idNode = child(idNode);
    }
    pSymbolItem sym = createSymbolItem(symbolTable->scope->currentDepth,
                                       createFieldNode(child(idNode)->info, NULL));
    if (strcmp(child(node)->name, "ID") == 0) {
        sym->field->type = cloneSType(specType);
    } else {
        pNode temp = child(node);
        pSType curType = specType;
        while (next(temp)) {
            sym->field->type = createSType(ARRAY, cloneSType(curType), atoi(next(next(temp))->info));
            curType = sym->field->type;
            temp = child(temp);
        }
    }
    return sym;
}


/*
FunDec → ID LP VarList RP
| ID LP RP
*/
void processFunDec(pNode node, pSType retType) {
    assert(node != NULL);
    pSymbolItem funcSym = createSymbolItem(symbolTable->scope->currentDepth,
                        createFieldNode(child(node)->info, 
                        createSType(FUNC, 0, NULL, cloneSType(retType))));
    if (strcmp(next(next(child(node)))->name, "VarList") == 0) {
        processVarList(next(next(child(node))), funcSym);
    }
    if (lookupSymbol(symbolTable, funcSym->field->name) != NULL) {
        char msg[100] = {0};
        sprintf(msg, "Redefined function \"%s\".", funcSym->field->name);
        pError(REDEF_FUNC, node->line, msg);
        freeSymbolItem(funcSym);
    } else {
        addSymbolItem(symbolTable, funcSym);
    }
}

/*
VarList → ParamDec COMMA VarList
| ParamDec
*/
void processVarList(pNode node, pSymbolItem funcSym) {
    assert(node != NULL);
    addScopeDepth(symbolTable->scope);
    int count = 0;
    pNode cur = child(node);
    pFieldNode argHead = processParamDec(cur);
    count++;
    funcSym->field->type->u.funcType.argList = cloneFieldNode(argHead);
    pFieldNode curField = funcSym->field->type->u.funcType.argList;
    while (next(cur)) {
        cur = next(next(cur));
        pFieldNode newArg = processParamDec(cur);
        curField->next = cloneFieldNode(newArg);
        curField = curField->next;
        count++;
    }
    funcSym->field->type->u.funcType.argCount = count;
    decreaseScopeDepth(symbolTable->scope);
}

// ParamDec → Specifier VarDec
pFieldNode processParamDec(pNode node) {
    assert(node != NULL);
    pSType paramType = processSpecifier(child(node));
    pSymbolItem paramSym = processVarDec(next(child(node)), paramType);
    if (lookupSymbol(symbolTable, paramSym->field->name) != NULL) {
        char msg[100] = {0};
        sprintf(msg, "Redefined parameter \"%s\".", paramSym->field->name);
        pError(REDEF_VAR, node->line, msg);
        freeSymbolItem(paramSym);
        return NULL;
    } else {
        addSymbolItem(symbolTable, paramSym);
        return paramSym->field;
    }
}

// CompSt → LC DefList StmtList RC
void processCompSt(pNode node, pSType retType) {
    assert(node != NULL);
    addScopeDepth(symbolTable->scope);
    pNode cur = next(child(node)); // 跳过左大括号
    if (strcmp(cur->name, "DefList") == 0) {
        processDefList(cur, NULL);
        cur = next(cur);
    }
    if (strcmp(cur->name, "StmtList") == 0) {
        processStmtList(cur, retType);
    }
    removeScopeSymbols(symbolTable);
}

void processStmtList(pNode node, pSType retType) {
    while (node) {
        processStmt(child(node), retType);
        node = next(child(node));
    }
}

void processStmt(pNode node, pSType retType) {
    assert(node != NULL);
    pSType expType = NULL;
    if (strcmp(child(node)->name, "Exp") == 0) {
        expType = processExp(child(node));
    } else if (strcmp(child(node)->name, "CompSt") == 0) {
        processCompSt(child(node), retType);
    } else if (strcmp(child(node)->name, "RETURN") == 0) {
        expType = processExp(next(child(node)));
        if (!checkSTypeEqual(retType, expType))
            pError(TYPE_MISMATCH_RETURN, node->line, "Return type mismatch.");
    } else if (strcmp(child(node)->name, "IF") == 0) {
        pNode stmtNode = next(next(next(next(child(node)))));
        expType = processExp(next(next(child(node))));
        processStmt(stmtNode, retType);
        if (next(stmtNode))
            processStmt(next(next(stmtNode)), retType);
    } else if (strcmp(child(node)->name, "WHILE") == 0) {
        expType = processExp(next(next(child(node))));
        processStmt(next(next(next(next(child(node))))), retType);
    }
    if (expType) freeSType(expType);
}

/*
DefList → Def DefList
| NULL
*/
void processDefList(pNode node, pSymbolItem structSym) {
    while (node) {
        processDef(child(node), structSym);
        node = next(child(node));
    }
}

// Def → Specifier DecList SEMI
void processDef(pNode node, pSymbolItem structSym) {
    assert(node != NULL);
    pSType defType = processSpecifier(child(node));
    processDecList(next(child(node)), defType, structSym);
    if (defType) freeSType(defType);
}

/*
 DecList → Dec
| Dec COMMA DecList
*/
void processDecList(pNode node, pSType specType, pSymbolItem structSym) {
    assert(node != NULL);
    pNode cur = node;
    while (cur) {
        processDec(child(cur), specType, structSym);
        if (child(cur)->children[1])
            cur = child(cur)->children[1];
        else
            break;
    }
}

/*
Dec → VarDec
| VarDec ASSIGNOP Exp
*/
void processDec(pNode node, pSType specType, pSymbolItem structSym) {
    assert(node != NULL);
    if (next(child(node)) == NULL) {
        if (structSym != NULL) {
            pSymbolItem decSym = processVarDec(child(node), specType);
            pFieldNode curField = structSym->field->type->u.structType.fields;
            pFieldNode lastField = NULL;
            while (curField) {
                if (strcmp(decSym->field->name, curField->name) == 0) {
                    char msg[100] = {0};
                    sprintf(msg, "Redefined field \"%s\".", decSym->field->name);
                    pError(REDEF_FIELD, node->line, msg);
                    freeSymbolItem(decSym);
                    return;
                }
                lastField = curField;
                curField = curField->next;
            }
            if (lastField == NULL)
                structSym->field->type->u.structType.fields = cloneFieldNode(decSym->field);
            else
                lastField->next = cloneFieldNode(decSym->field);
            freeSymbolItem(decSym);
        } else {
            pSymbolItem decSym = processVarDec(child(node), specType);
            if (lookupSymbol(symbolTable, decSym->field->name) != NULL) {
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".", decSym->field->name);
                pError(REDEF_VAR, node->line, msg);
                freeSymbolItem(decSym);
            } else {
                addSymbolItem(symbolTable, decSym);
            }
        }
    } else {
        if (structSym != NULL) {
            pError(REDEF_FIELD, node->line, "Illegal initialization in struct.");
        } else {
            pSymbolItem decSym = processVarDec(child(node), specType);
            pSType expType = processExp(next(next(child(node))));
            if (lookupSymbol(symbolTable, decSym->field->name) != NULL) {
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".", decSym->field->name);
                pError(REDEF_VAR, node->line, msg);
                freeSymbolItem(decSym);
            }
            if (!checkSTypeEqual(decSym->field->type, expType)) {
                pError(TYPE_MISMATCH_ASSIGN, node->line, "Assignment type mismatch.");
                freeSymbolItem(decSym);
            }
            if (decSym->field->type && decSym->field->type->kind == ARRAY) {
                pError(TYPE_MISMATCH_ASSIGN, node->line, "Illegal initialization for array.");
                freeSymbolItem(decSym);
            } else {
                addSymbolItem(symbolTable, decSym);
            }
            if (expType) freeSType(expType);
        }
    }
}

/*
Exp → Exp ASSIGNOP Exp
| Exp AND Exp
| Exp OR Exp
| Exp RELOP Exp
| Exp PLUS Exp
| Exp MINUS Exp
| Exp STAR Exp
| Exp DIV Exp
| LP Exp RP
| MINUS Exp
| NOT Exp
| ID LP Args RP
| ID LP RP
| Exp LB Exp RB
| Exp DOT ID
| ID
| INT
| FLOAT
*/
pSType processExp(pNode node) {
    assert(node != NULL);
    pNode t = child(node);
    if (strcmp(t->name, "Exp") == 0) {
        
        if (strcmp(next(t)->name, "LB") != 0 && strcmp(next(t)->name, "DOT") != 0) {
            /*
            Exp → Exp ASSIGNOP Exp
            | Exp AND Exp
            | Exp OR Exp
            | Exp RELOP Exp
            | Exp PLUS Exp
            | Exp MINUS Exp
            | Exp STAR Exp
            | Exp DIV Exp
            */
            pSType leftType = processExp(t);
            pSType rightType = processExp(next(next(t)));
            pSType resultType = NULL;
            if (strcmp(next(t)->name, "ASSIGNOP") == 0) {
                pNode leftChild = child(t);
                if (strcmp(leftChild->name, "FLOAT") == 0 || strcmp(leftChild->name, "INT") == 0) {
                    pError(LEFT_VAR_ASSIGN, t->line, "Left-hand side is not assignable.");
                } else if ((strcmp(leftChild->name, "ID") == 0 && leftChild->next == NULL) || 
                           strcmp(next(leftChild)->name, "LB") == 0 ||
                           strcmp(next(leftChild)->name, "DOT") == 0) {
                    /*
                    | Exp LB Exp RB
                    | Exp DOT ID
                    | ID
                    */
                    if (!checkSTypeEqual(leftType, rightType)) {
                        pError(TYPE_MISMATCH_ASSIGN, t->line, "Assignment type mismatch.");
                    } else {
                        resultType = cloneSType(leftType);
                    }
                } else {
                    pError(LEFT_VAR_ASSIGN, t->line, "Left-hand side is not a valid variable.");
                }
            } else {
                if ((leftType && leftType->kind == ARRAY) || (rightType && rightType->kind == ARRAY)) {
                    pError(TYPE_MISMATCH_OP, t->line, "Illegal operand for operator.");
                } else if (!checkSTypeEqual(leftType, rightType)) {
                    pError(TYPE_MISMATCH_OP, t->line, "Operand types do not match.");
                } else {
                    if (leftType && rightType) {
                        resultType = cloneSType(leftType);
                    }
                }
            }
            if (leftType) freeSType(leftType);
            if (rightType) freeSType(rightType);
            return resultType;
        } else {
            /*
            | Exp LB Exp RB
            | Exp DOT ID
            */
            if (strcmp(next(t)->name, "LB") == 0) {
                pSType arrayType = processExp(t);
                pSType indexType = processExp(next(t)->children[0]);
                pSType resType = NULL;
                if (arrayType == NULL) {
                } else if (arrayType->kind != ARRAY) {
                    char msg[100] = {0};
                    pError(NOT_A_ARRAY, t->line, "The variable is not an array.");
                } else if (indexType == NULL || indexType->kind != BASIC || indexType->u.basicType != INT_TYPE) {
                    char msg[100] = {0};
                    pError(NON_INT_INDEX, t->line, "The index of array is not an integer.");
                } else {
                    resType = cloneSType(arrayType->u.array.elemType);
                }
                if (arrayType) freeSType(arrayType);
                if (indexType) freeSType(indexType);
                return resType;
            } else {
                pSType structType = processExp(t);
                pSType resType = NULL;
                if (structType == NULL || structType->kind != STRUCT || structType->u.structType.tag == NULL) {
                    pError(NOT_A_STRUCT, t->line, "Illegal use of '.' operator.");
                    if (structType) freeSType(structType);
                } else {
                    pNode fieldNode = next(t);
                    pFieldNode curField = structType->u.structType.fields;
                    while (curField) {
                        if (strcmp(curField->name, fieldNode->info) == 0)
                            break;
                        curField = curField->next;
                    }
                    if (curField == NULL) {
                        pError(NONEXIST_FIELD, t->line, "Undefined field.");
                    } else {
                        resType = cloneSType(curField->type);
                    }
                }
                if (structType) freeSType(structType);
                return resType;
            }
        }
    } else if (strcmp(t->name, "MINUS") == 0 || strcmp(t->name, "NOT") == 0) {
        /*
        | MINUS Exp
        | NOT Exp
        */
        pSType operandType = processExp(next(t));
        pSType ret = NULL;
        if (operandType == NULL || operandType->kind != BASIC) {
            pError(UNDEF_VAR, t->line, "Illegal operand.");
        } else {
            ret = cloneSType(operandType);
        }
        if (operandType) freeSType(operandType);
        return ret;
    } else if (strcmp(t->name, "LP") == 0) {
        return processExp(next(t));
    } else if (strcmp(t->name, "ID") == 0 && next(t)) {
        pSymbolItem funcSym = lookupSymbol(symbolTable, t->info);
        if (funcSym == NULL) {
            char msg[100] = {0};
            sprintf(msg, "Undefined function \"%s\".", t->info);
            pError(UNDEF_FUNC, node->line, msg);
            return NULL;
        } else if (funcSym->field->type->kind != FUNC) {
            char msg[100] = {0};
            sprintf(msg, "\"%s\" is not a function.", t->info);
            pError(NOT_A_FUNC, node->line, msg);
            return NULL;
        } else if (strcmp(next(next(t))->name, "Args") == 0) {
            processArgs(next(next(t)), funcSym);
            return cloneSType(funcSym->field->type->u.funcType.retType);
        } else {
            if (funcSym->field->type->u.funcType.argCount != 0) {
                char msg[100] = {0};
                sprintf(msg, "Too few arguments for function \"%s\"; expected %d args.",
                        funcSym->field->name,
                        funcSym->field->type->u.funcType.argCount);
                pError(FUNC_ARG_MISMATCH, node->line, msg);
            }
            return cloneSType(funcSym->field->type->u.funcType.retType);
        }
    } else if (strcmp(t->name, "ID") == 0) {
        pSymbolItem varSym = lookupSymbol(symbolTable, t->info);
        if (varSym == NULL || isStructDefined(varSym)) {
            char msg[100] = {0};
            sprintf(msg, "Undefined variable \"%s\".", t->info);
            pError(UNDEF_VAR, t->line, msg);
            return NULL;
        } else {
            return cloneSType(varSym->field->type);
        }
    } else {
        if (strcmp(t->name, "FLOAT") == 0)
            return createSType(BASIC, FLOAT_TYPE);
        else
            return createSType(BASIC, INT_TYPE);
    }
}

void processArgs(pNode node, pSymbolItem funcSym) {
    assert(node != NULL);
    pNode cur = node;
    pFieldNode argDef = funcSym->field->type->u.funcType.argList;
    while (cur) {
        if (argDef == NULL) {
            char msg[100] = {0};
            sprintf(msg, "Too many arguments for function \"%s\"; expected %d args.",
                    funcSym->field->name, funcSym->field->type->u.funcType.argCount);
            pError(FUNC_ARG_MISMATCH, node->line, msg);
            break;
        }
        pSType argRealType = processExp(child(cur));
        if (!checkSTypeEqual(argRealType, argDef->type)) {
            char msg[100] = {0};
            sprintf(msg, "Function \"%s\" argument type mismatch.",
                    funcSym->field->name);
            pError(FUNC_ARG_MISMATCH, node->line, msg);
            if (argRealType) freeSType(argRealType);
            return;
        }
        if (argRealType) freeSType(argRealType);
        argDef = argDef->next;
        if (child(cur)->children[1])
            cur = child(cur)->children[1];
        else
            break;
    }
    if (argDef != NULL) {
        char msg[100] = {0};
        sprintf(msg, "Too few arguments for function \"%s\"; expected %d args.",
                funcSym->field->name, funcSym->field->type->u.funcType.argCount);
        pError(FUNC_ARG_MISMATCH, node->line, msg);
    }
}

pSymbolItem getScopeHead(pSymStack stack) {
    assert(stack != NULL);
    return stack->stackArray[stack->currentDepth];
}

void setScopeHead(pSymStack stack, pSymbolItem head) {
    assert(stack != NULL);
    stack->stackArray[stack->currentDepth] = head;
}

void addScopeDepth(pSymStack stack) {
    assert(stack != NULL);
    stack->currentDepth++;
    stack->stackArray[stack->currentDepth] = NULL;
}

void decreaseScopeDepth(pSymStack stack) {
    assert(stack != NULL);
    stack->stackArray[stack->currentDepth] = NULL;
    stack->currentDepth--;
}

pSymStack initScopeStack(void) {
    pSymStack stack = (pSymStack)malloc(sizeof(SymStack));
    assert(stack != NULL);
    stack->currentDepth = 0;
    stack->stackArray = (pSymbolItem*)malloc(sizeof(pSymbolItem) * HASH_TABLE_SIZE);
    assert(stack->stackArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        stack->stackArray[i] = NULL;
    }
    return stack;
}

void freeScopeStack(pSymStack stack) {
    assert(stack != NULL);
    free(stack->stackArray);
    free(stack);
}

boolean isStructDefined(pSymbolItem item) {
    assert(item != NULL);
    return item->field->type->kind == STRUCT && item->field->type->u.structType.tag != NULL;
}
