#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbol_table.h"

/* Fonction pour crée la table de symbole vide
 * @return: une table de symbole
 */
SymbolTable* symtab_create() {
    SymbolTable *t = malloc(sizeof(SymbolTable));
    t->schema_count = 0;
    t->source_count = 0;
    return t;
}


// pr les sources:

/* Fonction qui cherche l'indice de la source à partir de son nom 
 * @param t: pointeur vers une table de symbole
 * @param name: pointeur vers une chaine de caractère representant le nom de la source
 * @return: Si le nom de la source est trouvé on renvoie l'indice, -1 sinon
 */
static int source_index(SymbolTable *t, const char *name) {
    for (int i = 0; i < t->source_count; i++) {
        if (strcmp(t->sources[i].name, name) == 0) return i;
    }
    return -1;
}

int symtab_add_source(SymbolTable *t, const char *name, const char *file) {
    if (!t || !name || !file) return 0;

    int i = source_index(t, name);
    if (i >= 0) {
        // update si déjà existante
        strncpy(t->sources[i].file, file, sizeof(t->sources[i].file)-1);
        t->sources[i].file[sizeof(t->sources[i].file)-1] = '\0';
        return 1;
    }

    if (t->source_count >= (int)(sizeof(t->sources)/sizeof(t->sources[0]))) return 0;

    Source *s = &t->sources[t->source_count++];
    strncpy(s->name, name, sizeof(s->name)-1);
    s->name[sizeof(s->name)-1] = '\0';
    strncpy(s->file, file, sizeof(s->file)-1);
    s->file[sizeof(s->file)-1] = '\0';
    return 1;
}

const char *symtab_get_source_file(SymbolTable *t, const char *name) {
    if (!t || !name) return NULL;
    int i = source_index(t, name);
    if (i < 0) return NULL;
    return t->sources[i].file;
}


void symtab_add_schema(SymbolTable *t, const char *schema, const char *source) {
    if (!t || !schema) return;
    if (t->schema_count >= (int)(sizeof(t->schemas)/sizeof(t->schemas[0]))) return;

    for (int i = 0; i < t->schema_count; i++) {
        if (strcmp(t->schemas[i].name, schema) == 0) return; // Ne rien faire si le schema existe
    }

    Schema *s = &t->schemas[t->schema_count++]; 
    memset(s, 0, sizeof(*s)); 
    strncpy(s->name, schema, sizeof(s->name)-1); //Copie le nom du schéma dans s qui est le meme que t
    s->name[sizeof(s->name)-1] = '\0';

    if (source) {
        strncpy(s->source, source, sizeof(s->source)-1); //copie nom de source de schema s'il existe
        s->source[sizeof(s->source)-1] = '\0';
    } else {
        s->source[0] = '\0';
    }

    s->column_count = 0; //Il n'y a pas de colonne ajouté pour l'instant
}

void symtab_add_field(SymbolTable *t, const char *schema_name, const char *field, const char *type) {
    if (!t || !schema_name || !field || !type) return;

    Schema *s = NULL;
    for (int i = 0; i < t->schema_count; i++) {
        if (strcmp(t->schemas[i].name, schema_name) == 0) {
            s = &t->schemas[i]; // on ajoutera une colonne a un schema avec un nom deja existant 
            break;
        }
    }
    if (!s) return; //c pour ne pas ajouter une colonne a un schema vide

    if (s->column_count >= (int)(sizeof(s->columns)/sizeof(s->columns[0]))) return; //si full ne pas ajouter

    for (int i = 0; i < s->column_count; i++) {
        if (strcmp(s->columns[i].name, field) == 0) return;
    }

    Column *c = &s->columns[s->column_count++];
    strncpy(c->name, field, sizeof(c->name)-1); //On copie le  nom a la colonne
    c->name[sizeof(c->name)-1] = '\0';
    strncpy(c->type, type, sizeof(c->type)-1); //et un type
    c->type[sizeof(c->type)-1] = '\0';
}

Schema *symtab_get_schema(SymbolTable *t, const char *schema_name) {
    if (!t || !schema_name) return NULL;
    for (int i = 0; i < t->schema_count; i++) {
        if (strcmp(t->schemas[i].name, schema_name) == 0) return &t->schemas[i];
    }
    return NULL;
}

Schema *symtab_get_schema_by_source(SymbolTable *t, const char *source_name) {
    if (!t || !source_name) return NULL;
    for (int i = 0; i < t->schema_count; i++) {
        if (strcmp(t->schemas[i].source, source_name) == 0) return &t->schemas[i];
    }
    return NULL;
}

const char *schema_get_col_type(Schema *schema, const char *column) {
    if (!schema || !column) return NULL;
    for (int i = 0; i < schema->column_count; i++) {
        if (strcmp(schema->columns[i].name, column) == 0) return schema->columns[i].type;
    }
    return NULL;
}

void symtab_associate(SymbolTable *t, const char *schema, const char *source) {
    if (!t || !schema || !source) return;
    Schema *s = symtab_get_schema(t, schema);
    if (!s) return;
    strncpy(s->source, source, sizeof(s->source)-1);
    s->source[sizeof(s->source)-1] = '\0';
}

void symtab_print(SymbolTable *t) {
    if (!t) return;
    printf("==== TABLE DES SYMBOLES ====\n");

    printf("Sources:\n");
    for (int i = 0; i < t->source_count; i++) {
        printf("  %s -> %s\n", t->sources[i].name, t->sources[i].file);
    }

    printf("Schemas:\n");
    for (int i = 0; i < t->schema_count; i++) {
        Schema *s = &t->schemas[i];
        printf("Schema %s (source %s):\n", s->name, s->source);
        for (int j = 0; j < s->column_count; j++) {
            printf("  %s : %s\n", s->columns[j].name, s->columns[j].type);
        }
    }
}

Schema *symtab_add_empty_schema(SymbolTable *t, const char *name) {
    if (!t || !name) return NULL;
    if (t->schema_count >= (int)(sizeof(t->schemas)/sizeof(t->schemas[0]))) return NULL;
    Schema *existing = symtab_get_schema(t, name);
    if (existing) return existing;

    Schema *s = &t->schemas[t->schema_count++];
    memset(s, 0, sizeof(*s));
    strncpy(s->name, name, sizeof(s->name)-1);
    s->name[sizeof(s->name)-1] = '\0';
    s->source[0] = '\0'; //source null
    s->column_count = 0; //pas de colonne
    return s;
}

int schema_add_column(Schema *s, const char *name, const char *type) {
    if (!s || !name || !type) return 0;
    if (s->column_count >= (int)(sizeof(s->columns)/sizeof(s->columns[0]))) return 0;

    for (int i = 0; i < s->column_count; i++)
        if (strcmp(s->columns[i].name, name) == 0) return 0;

    Column *c = &s->columns[s->column_count++];
    strncpy(c->name, name, sizeof(c->name)-1);
    c->name[sizeof(c->name)-1] = '\0';
    strncpy(c->type, type, sizeof(c->type)-1);
    c->type[sizeof(c->type)-1] = '\0';
    return 1;
}

Schema *symtab_clone_schema(SymbolTable *t, const char *new_name, Schema *base) {
    if (!t || !new_name || !base) return NULL;
    Schema *s = symtab_add_empty_schema(t, new_name);
    if (!s) return NULL;

    for (int i = 0; i < base->column_count; i++) {
        schema_add_column(s, base->columns[i].name, base->columns[i].type);
    }
    return s;
}
