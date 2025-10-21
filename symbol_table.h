#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *type;  // "source" ou "schema"
} Symbol;

typedef struct {
    Symbol **symbols;
    int n_symbols;
} SymbolTable;

SymbolTable *symtab_create();
void symtab_add(SymbolTable *st, const char *name, const char *type);
Symbol *symtab_find(SymbolTable *st, const char *name);
void symtab_print(SymbolTable *st);
void symtab_free(SymbolTable *st);

#endif

