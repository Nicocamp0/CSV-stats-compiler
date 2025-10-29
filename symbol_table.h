/**
 * @file symbol_table.h
 * @brief Gestion de la Table des Symboles pour le compilateur AFSTAT.
 *
 * Définit les structures et fonctions pour enregistrer les identifiants
 * déclarés (sources, schémas) et effectuer la vérification sémantique.
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Une source ou un schema a un nom et un type  */
typedef struct {
    char *name;
    char *type;
} Symbol;

// Un tableau dynamique de pointeurs de Symboles
typedef struct {
    Symbol **symbols;
    int n_symbols;
} SymbolTable;

/* Fonction qui alloue dynamiquement une table des symboles et l'initialise
 * @returns: un pointeur vers la nouvelle table des symboles
 */
SymbolTable *symtab_create();

/* Procédure qui ajoute un symbole à la table des symboles
 * @param1 st: la table des symboles
 * @param2 name: le nom du symbole
 * @param3 type: le type du symbole: "source" ou "schema"
 */
void symtab_add(SymbolTable *st, const char *name, const char *type);

/* Fonction qui cherche un symbole dans la table des symboles à partir de son nom
 * @param1 st: la table des symboles
 * @param2 name: le nom du symbole à chercher
 * @returns: un pointeur vers le symbole trouvé, ou NULL sinon
 */
Symbol *symtab_find(SymbolTable *st, const char *name);

/* Procédure qui affiche le contenu de la table des symboles, 
 * @param1 st: la table des symboles
 */
void symtab_print(SymbolTable *st);

/* Procédure qui libère la mémoire allouée pour la table des symboles
 * @param1 st: la table des symboles
 */
void symtab_free(SymbolTable *st);

#endif

