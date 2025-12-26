#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symbol_table.h"


int semantic_check(ASTNode *root, SymbolTable *symtab);

#endif

