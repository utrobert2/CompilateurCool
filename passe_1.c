
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "passe_1.h"
#include "miniccutils.h"

extern int trace_level;

// Fonction pour mettre à jour les champs type et global_decl et offset de la NODE_IDENT (occurrence de déclaration)
// node = la node sur laquelle on va faire la récursivité avec ses enfants
// global = booléen pour indiquer si on s'attaque aux variables globale ou locale (on le stockera dans node->global_decl) (global = true et local = false)
// typePerpetuate = type à stocker dans node->type
void analyse_decl_globloc(node_t node,bool global,node_type typePerpetuate){
    // On stock le type à mettre dans node->type dans une variable quelconque nommée typeInter
    node_type typeInter = typePerpetuate;
    if (node != NULL){
        // Si on se trouve sur un noeud de type DECLS, alors le fils droit est forcement le type qu'on va vouloir mettre dans node->type
        if (node->nature == NODE_DECLS){
            // Donc on stock ce dernier dans notre petite variable typeInter
            typeInter = node->opr[0]->type;
        }
    }

    // For sur les enfants de la node
    for (int i = 0;i < node->nops;i++){
        // Vérif de si l'enfant i est nul
        if (node->opr[i] == NULL) continue;
        // Récursivité avec les enfants de la node
        if (node->nature != NODE_PRINT) analyse_decl_globloc(node->opr[i],global,typeInter);

        // Premier cas pour NODE_IDENT (occurence de déclaration)
        if (node->nature == NODE_LIST && node->opr[i]->nature == NODE_IDENT){
            // On met à jour le champ global_decl et le champ type et l'offset avec la fonction env_add_element
            node->opr[i]->global_decl = global;
            node->opr[i]->type = typeInter;
            node->opr[i]->offset = env_add_element(node->opr[i]->ident,node->opr[i]);
        }
    }
    // Second cas pour NODE_IDENT (occurence de déclaration)
    if (node->nature == NODE_DECL && node->opr[0]->nature == NODE_IDENT){
        node->opr[0]->global_decl = global;
        node->opr[0]->type = typeInter;
        node->opr[0]->offset = env_add_element(node->opr[0]->ident,node->opr[0]);
    }
}

// Seconde fonction de descente, pour chercher les NODE_IDENT (occurrence d'utilisation) faisant référence à la node root
// Fonction quasi identique à descente1
void descente2(node_t node,node_t root){
    // For sur les enfants de la node
    for (int i = 0;i < node->nops;i++){

        // Vérif de si l'enfant i est nul
        if (node->opr[i] == NULL) continue;

        // Récursivité avec les enfants de la node
        descente2(node->opr[i],root);

        // Vérif de si on tombe sur une NODE_IDENT (utilisation) faisant référence à root
        if (node->opr[i] != NULL){
            if (node->opr[i]->nature == NODE_IDENT && node->opr[i]->type == TYPE_NONE){
                if (strcmp(node->opr[i]->ident,root->ident) == 0){
                    // Si c'est le cas, on update les champs type et decl_node
                    node->opr[i]->type = root->type;
                    node->opr[i]->decl_node = get_decl_node(root->ident);
                    node->opr[i]->decl_node = root;
                }
            }
        }
    }
}

// Fonction pour mettre à jour les champs type et decl_node des NODE_IDENT (occurence d'utilisation)
// node = node sur laquelle on va faire le parcours en profondeur récursif
// root = node sur laquelle on va faire le second parcours en profondeur récursif (qui va donc rester intact juqsqu'à trouver les NODE_IDENT (utilisation))
void descente1(node_t node,node_t root){
    // For sur les enfants de la node
    for (int i = 0;i < node->nops;i++){
        // Vérif de si l'enfant i est nul
        if (node->opr[i] == NULL) continue;

        // Récursivité avec les enfants de la node
        descente1(node->opr[i],root);

        // Vérification de si l'enfant i est une NODE_IDENT (occurence de déclaration)
        if (node->opr[i] != NULL){
            if (node->opr[i]->nature == NODE_IDENT && node->opr[i]->type != TYPE_NONE){

                // Si c'est le cas, on appelle la seconde fonction de descente en inversant root et node, node->opr[i] devenant la référence qui ne sera pas modifiée
                // et root devenant la node sur laquelle on va parcourir en profondeur une seconde fois pour chercher les NODE_IDENT (occurence d'utilisation)
                descente2(root,node->opr[i]);
            }
        }
    }
}


// Fonction d'analyse des expressions, mettant à jour le champ type des noeuds à l'origine d'expressions
void analyse_expr(node_t node){
    // Récursivité sur les enfants de la node
    for (int i = 0;i < node->nops;i++){
        if (node->opr[i] == NULL) continue;
        analyse_expr(node->opr[i]);
    }

    // Switch case sur la nature de la node, selon cette dernière le type sera TYPE_INT ou TYPE_BOOL
    switch (node->nature){
        case NODE_UMINUS:
            node->type = TYPE_INT;
            break;
        case NODE_BNOT:
            node->type = TYPE_INT;
            break;
        case NODE_PLUS:
            node->type = TYPE_INT;
            break;
        case NODE_MINUS:
            node->type = TYPE_INT;
            break;
        case NODE_MUL:
            node->type = TYPE_INT;
            break;
        case NODE_DIV:
            node->type = TYPE_INT;
            break;
        case NODE_MOD:
            node->type = TYPE_INT;
            break;
        case NODE_BAND:
            node->type = TYPE_INT;
            break;
        case NODE_BOR:
            node->type = TYPE_INT;
            break;
        case NODE_BXOR:
            node->type = TYPE_INT;
            break;
        case NODE_SLL:
            node->type = TYPE_INT;
            break;
        case NODE_SRL:
            node->type = TYPE_INT;
            break;
        case NODE_SRA:
            node->type = TYPE_INT;
            break;
        case NODE_NOT:
            node->type = TYPE_BOOL;
            break;
        case NODE_LT:
            node->type = TYPE_BOOL;
            break;
        case NODE_GT:
            node->type = TYPE_BOOL;
            break;
        case NODE_GE:
            node->type = TYPE_BOOL;
            break;
        case NODE_LE:
            node->type = TYPE_BOOL;
            break;
        case NODE_EQ:
            node->type = TYPE_BOOL;
            break;
        case NODE_NE:
            node->type = TYPE_BOOL;
            break;
        case NODE_AND:
            node->type = TYPE_BOOL;
            break;
        case NODE_OR:
            node->type = TYPE_BOOL;
            break;
        // Cas spécial, NODE_AFFECT prendra le type de son enfant droit (dans une expression exemple "a = b" on veut forcément que l'égalité se fasse selon le type le de a)
        case NODE_AFFECT:
            node->type = node->opr[0]->type;
            break;
    }
}

// Fonction de mise à jour du champ offset de la NODE_FUNC avec parcours en profondeur récursif
void offset_func(node_t node){
    // Si la node est non nulle et si sa nature est NODE_FUNC
    if (node != NULL){
        if (node->nature == NODE_FUNC){
            // Alors on met dans son offset l'offset actuel de l'environnement
            node->offset = get_env_current_offset();
        }
    }
    // For sur les enfants de la node
    for (int i = 0;i < node->nops;i++){
        // Vérif de si l'enfant i de la node est nulle
        if (node->opr[i] == NULL) continue;
        // Récursivité sur l'enfant i
        offset_func(node->opr[i]);
    }
}

void analyse_passe_1(node_t root){
    printf("\n************************************************************");
    printf("\n***********************ENTREE PASSE 1***********************");
    printf("\n************************************************************\n");
    printf("Entrée modif NODE_IDENT -> global_decl + type (déclaration) + offset\n");
    printf("Entrée modif NODE_IDENT -> local_decl + type (déclaration) + offset\n");
    printf("Entrée modif NODE_IDENT -> decl_node + type (utilisation)\n");
    printf("Entrée modif NODE_y -> type\n");

    reset_env_current_offset();
    push_global_context();
    analyse_decl_globloc(root->opr[0],true,TYPE_NONE);

    reset_env_current_offset();    
    push_context();
    analyse_decl_globloc(root->opr[1],false,TYPE_NONE);
    pop_context();
    
    offset_func(root);
    descente1(root,root);
    analyse_expr(root);
}





/*
void analyse_inst(node_t node){
    switch (node->nature){
        case NODE_IF:
            if (analyse_expr(node->opr[0]) != 2){
                exit(1);
            }
        case NODE_WHILE:
            if (analyse_expr(node->opr[0]) != 2){
                exit(1);
            }
        case NODE_FOR:

        case NODE_DOWHILE:
            if (analyse_expr(node->opr[0]) != 2){
                exit(1);
            }
    }
}
*/