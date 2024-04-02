
#include <stdio.h>

#include "defs.h"
#include "passe_1.h"

extern int trace_level;

void analyse_passe_1(node_t root) {

}
  
bool analyse_expr(node_t node){


    switch (node->nature){
        case NODE_IDENT:
        case NODE_INTVAL:
        case NODE_BOOLVAL:
        case NODE_STRINGVAL:
        case NODE_TYPE:
        case NODE_LIST:
        case NODE_PROGRAM:
        case NODE_BLOCK:
        case NODE_DECLS:
        case NODE_DECL:
        case NODE_IF:
        case NODE_WHILE:
        case NODE_FOR:
        case NODE_DOWHILE:
        case NODE_PRINT:
        case NODE_FUNC:
        case NODE_PLUS:
        case NODE_MINUS:
        case NODE_MUL:
        case NODE_DIV:
        case NODE_MOD:
        case NODE_LT:
        case NODE_GT:
        case NODE_LE:
        case NODE_GE:
        case NODE_EQ:
        case NODE_NE:
        case NODE_AND:
        case NODE_OR:
        case NODE_BAND:
        case NODE_BOR:
        case NODE_BXOR:
        case NODE_SRA:
        case NODE_SRL:
        case NODE_SLL:
        case NODE_NOT:
        case NODE_BNOT:
        case NODE_UMINUS:
        case NODE_AFFECT:
        default:{
            break;
        }
    }

}
