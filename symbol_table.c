#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symbol_table.h"

#define ST_MAX 1024

static char *symbols[ST_MAX];
static int   count = 0;

void st_add(const char *name) {
    if (count >= ST_MAX) {
        fprintf(stderr, "[SYMTAB] Table pleine, impossible d’ajouter %s\n", name);
        return;
    }
    symbols[count++] = strdup(name);
}

int st_exists(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(symbols[i], name) == 0)
            return 1;
    }
    return 0;
}

void st_print(void) {
    printf("=== Table des symboles ===\n");
    for (int i = 0; i < count; i++) {
        printf("  %s\n", symbols[i]);
    }
}

