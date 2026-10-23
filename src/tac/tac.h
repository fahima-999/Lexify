/* Lexify Three Address Code generator (manual 4.6) */
#ifndef LEXIFY_TAC_H
#define LEXIFY_TAC_H

#include <stdio.h>
#include "ast/ast.h"

/* Walks the (already semantically checked) AST and writes Three Address
 * Code to `out`. semantic_check() must run first: it fills Node.type on
 * every expression, which the generator uses to insert (float) casts. */
void tac_generate(const Node *root, FILE *out);

#endif
