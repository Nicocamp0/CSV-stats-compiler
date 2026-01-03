#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbol_table.h"

SymbolTable* symtab_create() {
    SymbolTable *t = malloc(sizeof(SymbolTable));
    t->schema_count = 0;
    return t;
}

void symtab_add_source(SymbolTable *t, const char *name) {
    (void)t;
    (void)name;
}


void symtab_add_schema(SymbolTable *t, const char *schema, const char *source) {
    Schema *s = &t->schemas[t->schema_count++];
    strcpy(s->name, schema);
    strcpy(s->source, source);
    s->column_count = 0;
}

void symtab_add_field(SymbolTable *t, const char *schema_name,
                      const char *field, const char *type)
{
    for (int i = 0; i < t->schema_count; i++) {
        Schema *s = &t->schemas[i];
        if (strcmp(s->name, schema_name) == 0) {
            Column *c = &s->columns[s->column_count++];
            strcpy(c->name, field);
            strcpy(c->type, type);
            return;
        }
    }
}

Schema *symtab_get_schema(SymbolTable *t, const char *schema_name) {
    for (int i = 0; i < t->schema_count; i++)
        if (strcmp(t->schemas[i].name, schema_name) == 0)
            return &t->schemas[i];
    return NULL;
}

Schema *symtab_get_schema_by_source(SymbolTable *t, const char *source_name) {
    for (int i = 0; i < t->schema_count; i++)
        if (strcmp(t->schemas[i].source, source_name) == 0)
            return &t->schemas[i];
    return NULL;
}

const char *schema_get_col_type(Schema *schema, const char *column)
{
    for (int i = 0; i < schema->column_count; i++)
        if (strcmp(schema->columns[i].name, column) == 0)
            return schema->columns[i].type;
    return NULL;
}

void symtab_print(SymbolTable *t) {
    printf("==== TABLE DES SYMBOLES ====\n");
    for (int i = 0; i < t->schema_count; i++) {
        Schema *s = &t->schemas[i];
        printf("Schema %s (source %s):\n", s->name, s->source);

        for (int j = 0; j < s->column_count; j++) {
            printf("  %s : %s\n", 
                s->columns[j].name,
                s->columns[j].type);
        }
    }
}

void symtab_associate(SymbolTable *t, const char *schema, const char *source) {
    Schema *s = symtab_get_schema(t, schema);
    if (s) strcpy(s->source, source);
}
Schema *symtab_add_empty_schema(SymbolTable *t, const char *name) {
    Schema *s = &t->schemas[t->schema_count++];
    strcpy(s->name, name);
    strcpy(s->source, "");     // dataset virtuel
    s->column_count = 0;
    return s;
}

int schema_add_column(Schema *s, const char *name, const char *type) {
    // refuse doublons
    for (int i = 0; i < s->column_count; i++)
        if (strcmp(s->columns[i].name, name) == 0) return 0;

    Column *c = &s->columns[s->column_count++];
    strcpy(c->name, name);
    strcpy(c->type, type);
    return 1;
}

Schema *symtab_clone_schema(SymbolTable *t, const char *new_name, Schema *base) {
    Schema *s = symtab_add_empty_schema(t, new_name);
    for (int i = 0; i < base->column_count; i++) {
        schema_add_column(s, base->columns[i].name, base->columns[i].type);
    }
    return s;
}


