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

/* Fonction qui ajoute une source dans la table de source qui est dans la table de symbole
    et remplace les éléments de la source si elle existe dejà
 * @param t: la table de symbole
 * @param name: Un pointeur vers le nom de la source 
 * @param file: Un pointeur vers le nom du fichier de la source
 * @return: 1 si l'operation est effectuer, 0 si la taille du tableau de source est full
 */
int symtab_add_source(SymbolTable *t, const char *name, const char *file);

/* Fonction qui cherche le nom de fichier de la source 
 * @param t: la table de symbole
 * @param name: Un pointeur vers le nom de la source 
 * @return: le nom du fichier de la source si trouvé, NULL sinon
 */
const char *symtab_get_source_file(SymbolTable *t, const char *name);

/* Schemas */

/* Procédure qui ajoute uns schema à la table de symbole
 * @param t: Pointeur vers la table de symbole
 * @param schema: Pointeur vers le nom du schema à ajouté
 * @param source: Pointeur vers le nom de la source 
 */
void symtab_add_schema(SymbolTable *t, const char *schema, const char *source);

/* Procédure qui ajoute la colonne a un schema qui existe deja dans la table de symbole
 * @param t: Pointeur vers la table de symbole
 * @param schema_name: Pointeur vers le nom du schema à ajouté
 * @param field: Pointeur vers le nom de la colonne
 * @param type: Pointeur vers le type de la colonne
 */
void symtab_add_field(SymbolTable *t, const char *schema_name, const char *field, const char *type);

/* Fonction qui cherche le schema de la table de symbole selon son nom
 * @param t: Pointeur vers la table de symbole
 * @param schema_name: Pointeur vers le nom du schema à cherché
 * @return: le schema de la table de symbole, ou NULL si pas trouvé
 */
Schema *symtab_get_schema(SymbolTable *t, const char *schema_name);

/* Fonction qui cherche le schema de la table de symbole par le nom de sa source
 * @param t: Pointeur vers la table de symbole
 * @param source_name: Pointeur vers le nom du la source du schéma recherché
 * @return: le schema de la table de symbole, ou NULL si pas trouvé
 */
Schema *symtab_get_schema_by_source(SymbolTable *t, const char *source_name);

/* Fonction qui cherche le type de la colonne d'un schema de la table de symbole
 * @param schema: Pointeur vers le nom d'un schema 
 * @param column: Pointeur vers le nom de la colonne de schema
 * @return char: le type de colonne de la table de symbole, ou NULL si pas trouvé
 */
const char *schema_get_col_type(Schema *schema, const char *column);

/* Procédure qui affiche la table de symbole
 * @param   t : un pointeur vers la table de symbole
 */
void symtab_print(SymbolTable *t);

/* Fonction qui ajoute une source a un schema de la table de symbole
 * @param   t : un pointeur vers la table de symbole
 * @param schema: Pointeur vers le nom d'un schema 
 * @param source: Pointeur vers le nom de la source
 * @return char: le type de colonne de la table de symbole, ou NULL si pas trouvé
 */
void symtab_associate(SymbolTable *t, const char *schema, const char *source);

/* Fonction qui ajoute une source a un schema de la table de symbole
 * @param   t : un pointeur vers la table de symbole
 * @param name: Pointeur vers le nom d'un schema 
 * @return Schema: schema ajouter, ou le schema trouvé sous le meme nom dans t
 */
Schema *symtab_add_empty_schema(SymbolTable *t, const char *name);

/* Fonction qui ajoute un shema qui existe dans la table de symbole avec un nouveau nom
 * @param   t : un pointeur vers la table de symbole
 * @param new_name: Pointeur vers le nom du nouveau schema 
 * @param base: Pointeur vers le schema dont on veut copié ces colonne dans le schéma cloné
 * @return Schema: le schema ajouté, NULL si on a pas pu le crée
 */
Schema *symtab_clone_schema(SymbolTable *t, const char *new_name, Schema *base);

/* Fonction qui ajoute une colonne a un schema 
 * @param   t : un pointeur vers la table de symbole
 * @param schema: Pointeur vers le nom d'un schema 
 * @param source: Pointeur vers le nom de la source
 * @return int: 1 si l'ajout est correcte, 0 s'il existe deja
 */
int schema_add_column(Schema *s, const char *name, const char *type);

#endif
