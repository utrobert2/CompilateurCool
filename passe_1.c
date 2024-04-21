#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "passe_1.h"
#include "miniccutils.h"

extern int trace_level;
static char* nameNode[] = {"NONE","NODE_PROGRAM","NODE_BLOCK","NODE_LIST","NODE_DECLS","NODE_DECL","NODE_IDENT","NODE_TYPE","NODE_INTVAL","NODE_BOOLVAL","NODE_STRINGVAL","NODE_FUNC","NODE_IF","NODE_WHILE","NODE_FOR","NODE_DOWHILE","NODE_PLUS","NODE_MINUS","NODE_MUL","NODE_DIV","NODE_MOD","NODE_LT","NODE_GT","NODE_LE","NODE_GE","NODE_EQ","NODE_NE","NODE_AND","NODE_OR","NODE_BAND","NODE_BOR","NODE_BXOR","NODE_NOT","NODE_BNOT","NODE_SLL","NODE_SRA","NODE_SRL","NODE_UMINUS","NODE_AFFECT","NODE_PRINT"};


/* Fonction pour mettre à jour les champs type et global_decl et offset de la NODE_IDENT (occurrence de déclaration)
 * node = la node sur laquelle on va faire la récursivité avec ses fils
 * global = booléen pour indiquer si on s'attaque aux variables globale ou locale (on le stockera dans node->global_decl) (global = true et local = false)
 * typePerpetuate = type à stocker dans node->type */
void analyse_decl_globloc(node_t node,bool global,node_type typePerpetuate){
    // Si la node est NULL, pas besoin de continuer la fonction
    if (node == NULL) return;
    
    // On stock le type à mettre dans node->type dans une variable quelconque nommée typeInter
    node_type typeInter = typePerpetuate;
    if (node != NULL){
        // Si on se trouve sur un noeud de type DECLS, alors le fils droit est forcement le type qu'on va vouloir mettre dans node->type de la ou des NODE_IDENT plus bas
        if (node->nature == NODE_DECLS){
            // Donc on stock ce dernier dans notre variable typeInter qui va nous permettre de perpétuer ce type dans les prochains appels récursifs
            typeInter = node->opr[0]->type;
        }
    }

    // For sur les fils de la node
    for (int i = 0;i < node->nops;i++){
        // Vérif de si le fils i est nul
        if (node->opr[i] == NULL) continue;
        // Récursivité avec les fils de la node (on ne touche pas au type des NODE_IDENT dans les NODE_PRINT)
        if (node->nature != NODE_PRINT) analyse_decl_globloc(node->opr[i],global,typeInter);
    }
    
    // Modification de NODE_IDENT (occurence de déclaration)
    if (node->nature == NODE_DECL && node->opr[0]->nature == NODE_IDENT){
        node->opr[0]->global_decl = global;
        node->opr[0]->type = typeInter;
        node->opr[0]->offset = env_add_element(node->opr[0]->ident,node->opr[0]);
    }
}

/* Fonction de descente, pour chercher les NODE_IDENT (occurrence d'utilisation) faisant référence à la node root */
void descente2(node_t node,node_t root){
    // Si la node est NULL, pas besoin de continuer la fonction
    if (node == NULL) return;
    
    // For sur les fils de la node
    for (int i = 0;i < node->nops;i++){
        // Vérif de si le fils i est nul
        if (node->opr[i] == NULL) continue;
        // Récursivité avec les fils de la node
        descente2(node->opr[i],root);
    }

    // Vérif de si on tombe sur une NODE_IDENT (occurrence d'utilisation) faisant référence à root (qui est une NODE_IDENT (occurrence de déclaration))
    if (node != NULL && root != NULL){
        if (node->nature == NODE_IDENT && node->type == TYPE_NONE && node->ident != NULL && root->ident != NULL){
            if (strcmp(node->ident,root->ident) == 0){
                // Si c'est le cas, on update les champs type et decl_node
                node->type = root->type;
                node->decl_node = root;
            }
        }
    }
}

/* Fonction d'analyse des expressions, mettant à jour le champ type des noeuds à l'origine d'expressions (et l'offset pour NODE_FUNC) et qui va aussi mettre
 * à jour les NODE_IDENT (occurrence d'utilisation) en appelant la fonction précédemment définie */
void analyse_expr(node_t node,node_t root){
    // Si la node est NULL, pas besoin de continuer la fonction
    if (node == NULL) return;
    // Récursivité sur les fils de la node
    for (int i = 0;i < node->nops;i++){
        if (node->opr[i] != NULL) analyse_expr(node->opr[i],root);
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
        // Cas spécial, NODE_AFFECT prendra le type de sone fils droit (dans une expression exemple "a = b" on veut forcément que l'égalité se fasse selon le type le de a)
        case NODE_AFFECT:
            node->type = node->opr[0]->type;
            break;
        case NODE_INTVAL:
            node->type = TYPE_INT;
            break;
        case NODE_BOOLVAL:
            node->type = TYPE_BOOL;
            break;
        case NODE_STRINGVAL:
            node->offset = add_string(node->str);
            break;
        case NODE_FUNC:
            node->offset = get_env_current_offset();
            node->opr[1]->type = node->opr[0]->type;
            break;
        // Si on tombe sur une NODE_IDENT (occurrence de déclaration) on appelle descente pour propager le type de cette dernière aux autres NODE_IDENT qui l'utilise 
        case NODE_IDENT:
            if (node->type != TYPE_NONE) descente2(root,node);
            break;
    }
}

/* Fonction de vérification de si une variable locale est affectée à elle-même lors de sa déclaration (ce qui est impossible) 
 * ident = le nom de la variable qui est en train d'être déclarée
 * node = la node sur laquelle on va appliquer le parcours en profondeur et qui si c'est une NODE_IDENT, on va comparer son nom au paramètre ident précédemment définie */
void checkup_inception(char * ident, node_t node){
    // Si la node est NULL, pas besoin de continuer la fonction
    if (node == NULL) return;
    
    // Récursivité sur les fils de la node
    for (int i = 0;i < node->nops;i++){
        if (node->opr[i] == NULL) continue;
        checkup_inception(ident,node->opr[i]);
    }
    
    // Si on tombe sur une NODE_IDENT, on compare son nom (son champ ident) avec l'ident mis en paramètre de la fonction checkup_inception
    if (node->nature == NODE_IDENT){
	if (strcmp(node->ident,ident) == 0){
	    // Si les deux sont identiques, c'est une erreur car on ne peut affecter une variable à elle-même alors qu'elle est en train d'être déclarée
	    fprintf(stderr, "Error line %d: context error\n", node->lineno);
	    exit(1);
	}
    }
}

/* Fonction de vérification des résultats des modifications précédentes */
void verif_context(node_t node){
    // Si la node est NULL, pas besoin de continuer la fonction
    if (node == NULL) return;
    
    // Récursivité sur les fils de la node
    for (int i = 0;i < node->nops;i++){
        if (node->opr[i] == NULL) continue;
        verif_context(node->opr[i]);
    }
    
    // Switch case sur la nature de la node, selon cette dernière on vérifie la présence d'erreurs contextuelle
    switch (node->nature){
        case NODE_PROGRAM:
            break;
        case NODE_BLOCK:
            break;
        case NODE_LIST:
            break;
        case NODE_DECLS:
            break;
        case NODE_DECL:
            if (node->opr[1] != NULL){
                if (node->opr[0]->type != node->opr[1]->type){
                    fprintf(stderr, "Error line %d: context error, type conflict\n", node->lineno);
                    exit(1);
                }
                if (node->opr[0]->global_decl == false) checkup_inception(node->opr[0]->ident,node->opr[1]);
                else{
                    if (node->opr[1]->nature != NODE_INTVAL && node->opr[1]->nature != NODE_BOOLVAL){
            	     	fprintf(stderr, "Error line %d: context error\n", node->lineno);
                    	exit(1);
            	     }
            	 }
            }    
            break;
        case NODE_IDENT:
            if (node->type == TYPE_NONE){
                fprintf(stderr, "Error line %d: context error\n", node->lineno);
                exit(1);
            }
            break;
        case NODE_TYPE:
            break;
        case NODE_INTVAL:
            break;
        case NODE_BOOLVAL:
            break;
        case NODE_STRINGVAL:
            break;
        case NODE_FUNC:
            if (strcmp(node->opr[1]->ident,"main") != 0){
                fprintf(stderr, "Error line %d: context error, function is not main\n", node->opr[1]->lineno);
                exit(1);
            }
            break;
        case NODE_IF:
            if (node->opr[0]->type != TYPE_BOOL){
                fprintf(stderr, "Error line %d: context error\n", node->opr[0]->lineno);
                exit(1);
            }
            break;
        case NODE_WHILE:
            if (node->opr[0]->type != TYPE_BOOL){
                fprintf(stderr, "Error line %d: context error, wrong declaration of arguments in while\n", node->opr[0]->lineno);
                exit(1);
            }
            break;
        case NODE_FOR:
            if (node->opr[0]->type != TYPE_INT || node->opr[2]->type != TYPE_INT || node->opr[1]->type != TYPE_BOOL){
                fprintf(stderr, "Error line %d: context error, wrong declaration of arguments in for\n", node->opr[0]->lineno);
                exit(1);
            }
            break;
        case NODE_DOWHILE:
            if (node->opr[1]->type != TYPE_BOOL){
                fprintf(stderr, "Error line %d: context error\n", node->lineno);
                exit(1);
            }
            break;
        case NODE_UMINUS:
        case NODE_NOT:
            if (node->opr[0]->type != TYPE_INT){
                fprintf(stderr, "Error line %d: context error !\n", node->lineno);
                exit(1);
            }
            break;
        case NODE_BNOT:
            if (node->opr[0]->type != TYPE_BOOL){
                fprintf(stderr, "Error line %d: context error\n", node->lineno);
                exit(1);
            }
            break;
        case NODE_PLUS:
        case NODE_MINUS:
        case NODE_MUL:
        case NODE_DIV:
        case NODE_MOD:
        case NODE_BAND:
        case NODE_BOR:
        case NODE_BXOR:
        case NODE_SLL:
        case NODE_SRL:
        case NODE_SRA:
        case NODE_LT:
        case NODE_GT:
        case NODE_GE:
        case NODE_LE:
        case NODE_EQ:
        case NODE_NE:
            if (node->opr[0]->type != TYPE_INT || node->opr[1]->type != TYPE_INT){
                fprintf(stderr, "Error line %d: context error\n", node->lineno);
                exit(1);
            } 
            break;
        case NODE_AND:
        case NODE_OR:
            if (node->opr[0]->type != TYPE_BOOL || node->opr[1]->type != TYPE_BOOL){
                fprintf(stderr, "Error line %d: context error\n", node->lineno);
                exit(1);
            } 
            break;
        case NODE_AFFECT:
            if (node->opr[0]->type != node->opr[1]->type){
                fprintf(stderr, "Error line %d: context error\n", node->lineno);
                exit(1);
            }
            break;
        case NODE_PRINT:
            break;
    }
}

void analyse_passe_1(node_t root){
    // Modification des champs des NODE_IDENT (occurrence de déclaration globale)
    reset_env_current_offset();
    push_global_context();
    analyse_decl_globloc(root->opr[0],true,TYPE_NONE);

    // Modification des champs des NODE_IDENT (occurrence de déclaration locale)
    reset_env_current_offset();    
    push_context();
    analyse_decl_globloc(root->opr[1],false,TYPE_NONE);
    pop_context();

    // Propagation des types et modification des champs des NODE_IDENT (occurrence d'utilisation)
    analyse_expr(root,root);

    // Vérification contextuelle
    verif_context(root);
} 
