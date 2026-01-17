#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"

typedef struct {
    char name[64];        // identifiant après "source"
    char file[256];       // chemin csv
} Source;

typedef struct {
    char name[64];
    char type[16];        // integer, float, string
} Column;

typedef struct {
    char name[64];
    char source[64];      // nom de la source associée
    Column columns[128];
    int column_count;
} Schema;

typedef struct {
    Source sources[64];
    int source_count;

    Schema schemas[32];
    int schema_count;
} SymbolTable;

/* ==== Fonctions ==== */

SymbolTable *symtab_create();

/* Sources */
int symtab_add_source(SymbolTable *t, const char *name, const char *file);
const char *symtab_get_source_file(SymbolTable *t, const char *name);

/* Schemas */
void symtab_add_schema(SymbolTable *t, const char *schema, const char *source);
void symtab_add_field(SymbolTable *t, const char *schema_name, const char *field, const char *type);

Schema *symtab_get_schema(SymbolTable *t, const char *schema_name);
Schema *symtab_get_schema_by_source(SymbolTable *t, const char *source_name);

const char *schema_get_col_type(Schema *schema, const char *column);

void symtab_print(SymbolTable *t);
void symtab_associate(SymbolTable *t, const char *schema, const char *source);

Schema *symtab_add_empty_schema(SymbolTable *t, const char *name);
Schema *symtab_clone_schema(SymbolTable *t, const char *new_name, Schema *base);
int schema_add_column(Schema *s, const char *name, const char *type);

#endif
