#include "symbol_table.h"

SymbolTable *symtab_create() {
    SymbolTable *st = (SymbolTable *)malloc(sizeof(SymbolTable));
    st->symbols = NULL;
    st->n_symbols = 0;
    return st;
}

void symtab_add(SymbolTable *st, const char *name, const char *type) {
    Symbol *s = (Symbol *)malloc(sizeof(Symbol));
    s->name = strdup(name);
    s->type = strdup(type);
    st->symbols = (Symbol **)realloc(st->symbols, sizeof(Symbol*)*(st->n_symbols+1));
    st->symbols[st->n_symbols++] = s;
}

Symbol *symtab_find(SymbolTable *st, const char *name) {
    for(int i=0;i<st->n_symbols;i++)
        if(strcmp(st->symbols[i]->name,name)==0)
            return st->symbols[i];
    return NULL;
}

void symtab_print(SymbolTable *st) {
    printf("=== Symbol Table ===\n");
    for(int i=0;i<st->n_symbols;i++)
        printf("%s : %s\n", st->symbols[i]->name, st->symbols[i]->type);
}

void symtab_free(SymbolTable *st) {
    for(int i=0;i<st->n_symbols;i++) {
        free(st->symbols[i]->name);
        free(st->symbols[i]->type);
        free(st->symbols[i]);
    }
    free(st->symbols);
    free(st);
}

