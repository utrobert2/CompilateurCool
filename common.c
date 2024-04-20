
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include "defs.h"
#include "common.h"
#include "arch.h"


extern char * infile;
extern char * outfile;
int32_t trace_level = DEFAULT_TRACE_LEVEL;
extern bool stop_after_syntax;
extern bool stop_after_verif;

// Fonction de traitement des arguments de la ligne de commande du compilateur (argc = nombre d'arguments, argv = les arguments)
void parse_args(int argc, char ** argv) {
    // Déclaration des variables activ_i (= 0 neutre, =1 activation de l'option, =-1 présence d'erreur)
    int activ_b = 0;
    int activ_t = 0;
    int activ_r = 0;
    int activ_s = 0;
    int activ_v = 0;
    int activ_h = 0;
    int activ_infile = 0;
    int activ_outfile = 0;
    
    // Déclaration des variables intermédiaire (elles garderont une valeur temporairement avant de la mettre dans les variables correspondante)
    int inter_t;
    int inter_r;
    char * interinfile = NULL;
    char * interoutfile = NULL;

    int k = 0;

    // On parcoure les arguments 
    for (int i = 0;i < argc;i++){
        // Option -h (activée si appelée ou si il n'y pas d'arguments en plus que ./nom_lambda)
        if ((strcmp(argv[i],"-h") == 0) || (argc == 1)){
            // On l'active
            activ_h = 1;
            // Si il y'a eu une erreur précédemment enregistrée, cette option ne sera pas activée donc neutre
            if ((activ_b == -1) || (activ_infile == -1) || (activ_outfile == -1) || (activ_r == -1) || (activ_s == -1) || (activ_t == -1) || (activ_v == -1)) activ_h = 0;
            // L'option arrête le parsing des arguments donc on quitte cette boucle for
            break;
        }

        // Option -h
        else if (strcmp(argv[i],"-b") == 0){
            // Si il y'a d'autres options/arguments on ne peut l'activer donc erreur
            if (argc > 2) activ_b = -1;
            // Sinon on l'active
            if (activ_b == 0) activ_b = 1;
        }

        // Option -s
        else if (strcmp(argv[i],"-s") == 0){
            // On vérifie la non présence de l'option -v (incompatible avec l'option -s)
            for (k = 0;k < argc;k++){
                // Si il y a présence de -v, erreur
                if (strcmp(argv[k],"-v") == 0){
                    activ_s = -1;
                }
            }
            // Sinon activation
            if (activ_s != -1) activ_s = 1;
        }

        // Option -v (même fonctionnement que -s)
        else if (strcmp(argv[i],"-v") == 0){
            for (k = 0;k < argc;k++){
                if (strcmp(argv[k],"-s") == 0){
                    activ_v = -1;
                }
            }
            if (activ_v != -1) activ_v = 1;
        }

        // Option -t
        else if (strcmp(argv[i],"-t") == 0){
            // Si il n'y a pas d'arguments après l'option, il y a erreur (cette option se déclare comme suit : -t <int>)
            if (i+1 == argc) activ_t = -1;
            // Seconde vérification de présence d'erreurs : si le <int> n'en est pas un ou si il se situe en dehors de l'intervalle requis
            else{
                char c = argv[i+1][strlen(argv[i+1])-1];
                if ((isalpha(c)) || (atoi(argv[i+1])) < 0 || (atoi(argv[i+1]) > 5)) activ_t = -1;
            }
            // Activation et collecte dans inter_t du <int>
            if (activ_t == 0){
                activ_t = 1;
                inter_t = atoi(argv[i+1]);
            }
        }

        // Option -r (même fonctionnement que -t)
        else if (strcmp(argv[i],"-r") == 0){
            if (i+1 == argc){ 
                activ_r = -1;
            }
            else{
                char c = argv[i+1][strlen(argv[i+1])-1];
                if ((isalpha(c)) || (atoi(argv[i+1]) < 4) || (atoi(argv[i+1]) > 8)) activ_r = -1;
            }
            if (activ_r == 0){
                activ_r = 1;
                inter_r = atoi(argv[i+1]);
            }
        }

        // Lors d'une rencontre d'un fichier .qqchose on vérifie son extension
        else if (argv[i][strlen(argv[i])-2] == '.'){
            // Fichier .c
            if (argv[i][strlen(argv[i])-1] == 'c'){
                // Si il y a déjà eu un fichier .c de rencontré précédemment c'est une erreur
                if (activ_infile == 1) activ_infile = -1;
                // Sinon activation et collecte du nom du fichier dans interinfile
                else if (activ_infile == 0){
                    activ_infile = 1;
                    interinfile = argv[i];
                }
            }
            // Fichier .s
            else if (argv[i][strlen(argv[i])-1] == 's'){
                // Vérification de si l'option -o a bien été spécifiée précédemment juste avant (pas de second fichier .s sinon erreur)
                if ((argv[i-1][strlen(argv[i-1])-1] != 'o') || (argv[i-1][strlen(argv[i-1])-2] != '-')){
                    activ_outfile = -1;
                }
            }
        }

        // Option -o
        else if (strcmp(argv[i],"-o") == 0){
            // On vérifie si il y'a bien un argument après l'option et si ce dernier est un fichier de type .s, si ce n'est pas le cas : erreur
            if ((i+1 == argc) || (argv[i+1][strlen(argv[i+1])-1] != 's') || (argv[i+1][strlen(argv[i+1])-2] != '.')){
                activ_outfile = -1;
            }
            // Sinon activation et collecte du nom du fichier dans interoutfile
            if (activ_outfile == 0){
                interoutfile = argv[i+1];
                activ_outfile = 1;
            }
        }
    }

    // Si aucun fichier .c n'a été déclaré et que les options -h et -b n'ont pas été déclarées, c'est une erreur
    if (interinfile == NULL && activ_b != 1 && activ_h != 1){
        printf("Error, missing a .c file\n");
        exit(1);
    }

    // On vérifie la présence d'erreurs parmi les variables d'activation
    int tab_activ[8] = {activ_b,activ_h,activ_infile,activ_outfile,activ_r,activ_s,activ_t,activ_v};
    for (int i = 0;i < 8;i++){
        if (tab_activ[i] == -1){
            printf("Error\n");
            exit(1);
        }
    }
    
    // Option -b : affichage du nom du compilateur et des membres du binôme
    if (activ_b == 1){
        printf("**************************** COMPILOTRON ****************************\n");
        printf("********************** BOUCHARD Paul - UT Robert ********************\n");
    }
    // Option -h : affichage de la liste des options
    if (activ_h == 1){
        printf("• -b : Affiche une bannière indiquant le nom du compilateur et des membres du binôme\n");
        printf("• -o <filename> : Définit le nom du fichier assembleur produit (défaut : out.s)\n");
        printf("• -t <int> : Définit le niveau de trace à utiliser entre 0 et 5 (0 = pas de trace ; 5 = toutes les traces. defaut = 0)\n");
        printf("• -r <int> : Définit le nombre maximum de registres à utiliser, entre 4 et 8 (défaut : 8)\n");
        printf("• -s : Arrêter la compilation après l'analyse syntaxique (défaut = non).\n");
        printf("• -v : Arrêter la compilation après la passe de vérifications (défaut = non)\n");
        printf("• -h : Afficher la liste des options (fonction d'usage) et arrêter le parsing des arguments\n");
        exit(0);
    }
    if (activ_infile == 1) infile = interinfile;
    if (activ_outfile == 1) outfile = interoutfile;
    if (activ_r == 1) set_max_registers(inter_r);
    if (activ_s == 1) stop_after_syntax = true;
    if (activ_t == 1) trace_level = inter_t;
    if (activ_v == 1) stop_after_verif = true;

    FILE * file = fopen(infile,"r");
    if (file == NULL){
        printf("Error : entry file non-existent\n");
        exit(1);
    }
    fclose(file);
}



void free_nodes(node_t n) {
    if (n == NULL) return;
    for (int i = 0;i < n->nops;i++){
        if (n->opr[i] != NULL){
            free_nodes(n->opr[i]);
            n->opr[i] = NULL;
        }
    }
    if (n->opr != NULL){
        free(n->opr);
        n->opr = NULL;
    }
    if (n->decl_node != NULL){
        //free(n->decl_node);
        n->decl_node = NULL;
    }
    if (n->ident != NULL) {
        free(n->ident);
        n->ident = NULL;
    }
    if (n->str != NULL) {
        free(n->str);
        n->str = NULL;
    }
    free(n);
}


char * strdupl(char * s) {
    char * r = malloc(strlen(s) + 1);
    strcpy(r, s);
    return r;
}


static int32_t dump_tree2dot_rec(FILE * f, node_t n, int32_t node_num) {

    if (n == NULL) {
        fprintf(f, "    N%d [shape=record, label=\"{{NULL}}\"];\n", node_num);
        return node_num;
    }

    switch (n->nature) {
        case NODE_IDENT:
            {
                node_t decl_node = n->decl_node;
                fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Type: %s}|{<decl>Decl      |Ident: %s|Offset: %d}}\"];\n", node_num, node_nature2string(n->nature), node_type2string(n->type), n->ident, n->offset);
                if (decl_node != NULL && decl_node != n) {
                    fprintf(f, "    edge[tailclip=false];\n");
                    fprintf(f, "    \"N%d\":decl:c -> \"N%d\" [style=dashed]\n", node_num, decl_node->node_num);
                }
                break;
            }
        case NODE_INTVAL:
        case NODE_BOOLVAL:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Type: %s}|{Value: %" PRId64 "}}\"];\n", node_num, node_nature2string(n->nature), node_type2string(n->type), n->value);
            break;
        case NODE_STRINGVAL:
            {
                char str[32];
                int32_t i = 1;
                while (true) {
                    str[i - 1] = n->str[i];
                    i += 1;
                    if (n->str[i] == '"') {
                        str[i - 1] = '\0';
                        break;
                    }
                }
                fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Type: %s}|{val: %s}}\"];\n", node_num, node_nature2string(n->nature), node_type2string(n->type), str);
            }
            break;
        case NODE_TYPE:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Type: %s}}\"];\n", node_num, node_nature2string(n->nature), node_type2string(n->type));
            break;
        case NODE_LIST:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE LIST}}\"];\n", node_num);
            break;
        case NODE_PROGRAM:
        case NODE_BLOCK:
        case NODE_DECLS:
        case NODE_DECL:
        case NODE_IF:
        case NODE_WHILE:
        case NODE_FOR:
        case NODE_DOWHILE:
        case NODE_PRINT:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Nb. ops: %d}}\"];\n", node_num, node_nature2string(n->nature), n->nops);
            break;
        case NODE_FUNC:
            fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Nb. ops: %d}|{offset: %d}}\"];\n", node_num, node_nature2string(n->nature), n->nops, n->offset);
            break;
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
            fprintf(f, "    N%d [shape=record, label=\"{{NODE %s|Type: %s|Nb. ops: %d}}\"];\n", node_num, node_nature2string(n->nature), node_type2string(n->type), n->nops);
            break;
        default:
            printf("*** Error in %s: unknow nature : %s\n", __func__, node_nature2string(n->nature));
            assert(false);
    }

    n->node_num = node_num;

    int32_t curr_node_num = node_num + 1;
    for (int32_t i = 0; i < n->nops; i += 1) {
        int32_t new_node_num = dump_tree2dot_rec(f, n->opr[i], curr_node_num);

        fprintf(f, "    edge[tailclip=true];\n");
        fprintf(f, "    N%d -> N%d\n", node_num, curr_node_num);
        curr_node_num = new_node_num + 1;
    }

    return curr_node_num - 1;
}



static void dump_tree2dot(FILE * f, node_t root) {
    assert(root->nature == NODE_PROGRAM);

    int32_t curr_node_num = 1;
    dump_tree2dot_rec(f, root, curr_node_num);
}


void dump_tree(node_t prog_root, const char * dotname) {

    FILE * f;

    f = fopen(dotname, "w");
    fprintf(f, "digraph global_vars {\n");
    dump_tree2dot(f, prog_root);
    fprintf(f, "}");    
    fclose(f);
}


const char * node_type2string(node_type t) {
    switch (t) {
        case TYPE_NONE:
            return "TYPE NONE";
        case TYPE_INT:
            return "TYPE INT";
        case TYPE_BOOL:
            return "TYPE BOOL";
        case TYPE_VOID:
            return "TYPE VOID";
        default:
            assert(false);
    }
}


const char * node_nature2string(node_nature t) {
    switch (t) {
        case NONE:
            return "NONE";
        case NODE_PROGRAM:
            return "PROGRAM";
        case NODE_BLOCK:
            return "BLOCK";
        case NODE_LIST:
            return "LIST";
        case NODE_DECLS:
            return "DECLS";
        case NODE_DECL:
            return "DECL";
        case NODE_IDENT:
            return "IDENT";
        case NODE_TYPE:
            return "TYPE";
        case NODE_INTVAL:
            return "INTVAL";
        case NODE_BOOLVAL:
            return "BOOLVAL";
        case NODE_STRINGVAL:
            return "STRINGVAL";
        case NODE_FUNC:
            return "FUNC";
        case NODE_IF:
            return "IF";
        case NODE_WHILE:
            return "WHILE";
        case NODE_FOR:
            return "FOR";
        case NODE_DOWHILE:
            return "DOWHILE";
        case NODE_PLUS:
            return "PLUS";
        case NODE_MINUS:
            return "MINUS";
        case NODE_MUL:
            return "MUL";
        case NODE_DIV:
            return "DIV";
        case NODE_MOD:
            return "MOD";
        case NODE_LT:
            return "LT";
        case NODE_GT:
            return "GT";
        case NODE_LE:
            return "LE";
        case NODE_GE:
            return "GE";
        case NODE_EQ:
            return "EQ";
        case NODE_NE:
            return "NE";
        case NODE_AND:
            return "AND";
        case NODE_OR:
            return "OR";
        case NODE_BAND:
            return "BAND";
        case NODE_BOR:
            return "BOR";
        case NODE_BXOR:
            return "BXOR";
        case NODE_SLL:
            return "SLL";
        case NODE_SRA:
            return "SRA";
        case NODE_SRL:
            return "SRL";
        case NODE_NOT:
            return "NOT";
        case NODE_BNOT:
            return "BNOT";
        case NODE_UMINUS:
            return "UMINUS";
        case NODE_AFFECT:
            return "AFFECT";
        case NODE_PRINT:
            return "PRINT";
        default:
            fprintf(stderr, "*** Error in %s: Unknown node nature: %d\n", __func__, t);
            exit(1);
    }
}




const char * node_nature2symb(node_nature t) {
    switch (t) {
        case NONE:
        case NODE_PLUS:
            return "+";
        case NODE_MINUS:
            return "-";
        case NODE_MUL:
            return "*";
        case NODE_DIV:
            return "/";
        case NODE_MOD:
            return "%";
        case NODE_LT:
            return "<";
        case NODE_GT:
            return ">";
        case NODE_LE:
            return "<=";
        case NODE_GE:
            return ">=";
        case NODE_EQ:
            return "==";
        case NODE_NE:
            return "!=";
        case NODE_AND:
            return "&&";
        case NODE_OR:
            return "||";
        case NODE_BAND:
            return "&";
        case NODE_BOR:
            return "|";
        case NODE_BXOR:
            return "^";
        case NODE_SRA:
            return ">>";
        case NODE_SRL:
            return ">>>";
        case NODE_SLL:
            return "<<";
        case NODE_NOT:
            return "!";
        case NODE_BNOT:
            return "~";
        case NODE_UMINUS:
            return "-";
        default:
            fprintf(stderr, "*** Error in %s: Unknown node nature: %d\n", __func__, t);
            exit(1);
    }
}




