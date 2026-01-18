#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "symbol_table.h"

// Génère un fichier C minimal (ou réel si CSV dispo) affichant les analyses demandées.
// Retourne 0 si OK, sinon 1.
int generate_c(ASTNode *root, SymbolTable *symtab, const char *out_c_path);

#endif
