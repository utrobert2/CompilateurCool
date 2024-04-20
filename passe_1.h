
#ifndef _PASSE_1_
#define _PASSE_1_

#include "defs.h"

void analyse_decl_globloc(node_t node,bool global,node_type typePerpetuate);
void descente2(node_t node,node_t root);
void analyse_expr(node_t node,node_t root);
void checkup_inception(char * ident, node_t node);
void verif_context(node_t node);
void analyse_passe_1(node_t root);

#endif

