#include <stdio.h>

#include "defs.h"
#include "passe_2.h"
#include "arch.h"
#include "miniccutils.h"

static char* nameNode[] = {"NONE","NODE_PROGRAM","NODE_BLOCK","NODE_LIST","NODE_DECLS","NODE_DECL","NODE_IDENT","NODE_TYPE","NODE_INTVAL","NODE_BOOLVAL","NODE_STRINGVAL","NODE_FUNC","NODE_IF","NODE_WHILE","NODE_FOR","NODE_DOWHILE","NODE_PLUS","NODE_MINUS","NODE_MUL","NODE_DIV","NODE_MOD","NODE_LT","NODE_GT","NODE_LE","NODE_GE","NODE_EQ","NODE_NE","NODE_AND","NODE_OR","NODE_BAND","NODE_BOR","NODE_BXOR","NODE_NOT","NODE_BNOT","NODE_SLL","NODE_SRA","NODE_SRL","NODE_UMINUS","NODE_AFFECT","NODE_PRINT"};

void search_string(){
    for (int i = 0; i < get_global_strings_number();i++){
        asciiz_inst_create(NULL, get_global_string(i));
    }
}

void gen_decl(node_t node){
    if (node == NULL) return;
    for (int i = 0;i < node->nops;i++){
        if (node->opr[i] == NULL) continue;
        gen_decl(node->opr[i]);

        if (node->nature == NODE_LIST && node->opr[i]->nature == NODE_IDENT){
            word_inst_create(node->opr[1]->ident,0);
        }
    }

    if (node->nature == NODE_DECL && node->opr[0]->nature == NODE_IDENT){
        word_inst_create(node->opr[0]->ident,node->opr[1]->value);
    }
}

void print_string(node_t node){
    if (node == NULL) return;
    for (int i = 0;i < node->nops;i++){
        if (node->opr[i] == NULL) continue;
        print_string(node->opr[i]);
    }

    switch (node->nature){
        case NODE_STRINGVAL:
            lui_inst_create(4,0x1001);
            ori_inst_create(4,4,node->offset);
            ori_inst_create(2,0,4);
            syscall_inst_create();
            break;
        case NODE_IDENT:
            if (node->global_decl){
            	lui_inst_create(4,0x1001);
            	lw_inst_create(4,node->decl_node->offset,4);
            	ori_inst_create(2,0,1);
            	syscall_inst_create();
            }
            else{
            	lw_inst_create(4,node->decl_node->offset,29);
            	ori_inst_create(2,0,1);
            	syscall_inst_create();
            }
            break;
    }
}



void gen_main(node_t node){
    int32_t lw = 0;
    if (node == NULL) return;
    for (int i = 0;i < node->nops;i++){
        if (node->opr[i] == NULL) continue;
        gen_main(node->opr[i]);
    }

    switch (node->nature){
        case NONE:
        case NODE_PROGRAM:
        case NODE_BLOCK:
        case NODE_LIST:
        case NODE_DECLS:
        case NODE_DECL:
            if (node->opr[1] != NULL){
                if (node->opr[1]->nature == NODE_IDENT){
                    lui_inst_create(8,0x1001);
                    lw_inst_create(8,lw,8);
                    sw_inst_create(8,node->opr[0]->offset,29);
                    lw = lw + 4;
                }
                else{
                    ori_inst_create(8,0,node->opr[1]->value);
                    sw_inst_create(8,node->opr[0]->offset,29);
                }
            }
        case NODE_IDENT:
        case NODE_TYPE:
        case NODE_INTVAL:
        case NODE_BOOLVAL:
        case NODE_STRINGVAL:
        case NODE_FUNC:
        case NODE_IF:
        case NODE_WHILE:
        case NODE_FOR:
        case NODE_DOWHILE:
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
        case NODE_NOT:
        case NODE_BNOT:
        case NODE_SLL:
        case NODE_SRA:
        case NODE_SRL:
        case NODE_UMINUS:
        case NODE_AFFECT:
        case NODE_PRINT:
            print_string(node);
            break;
    }
}

void gen_code_passe_2(node_t root){
/*
    // Création programme et section des données
    create_program();
    dump_mips_program("testasm");
    
    // Remplissage de la section .data
    comment_inst_create("Declaration des variables globales");
    data_sec_inst_create();
    search_string();
    gen_decl(root->opr[0]);

    // Création de la section .text et remplissage du main
    text_sec_inst_create();
    label_str_inst_create("main");
    
    reset_temporary_max_offset();
    set_temporary_start_offset(get_env_current_offset());
    stack_allocation_inst_create();

    //gen_main(root->opr[1]);


    stack_deallocation_inst_create(get_temporary_max_offset() + root->opr[1]->offset);
    ori_inst_create(2,0,10);
    syscall_inst_create();
    free_global_strings();
    free_program();
*/
}
  

