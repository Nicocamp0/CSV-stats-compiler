# Makefile minimaliste pour AFSTAT

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -g
LEX = flex
YACC = bison
LEX_FILE = lexer.l
YACC_FILE = parser.y
TARGET = analyseur

# Fichiers générés
LEX_OUT = lex.yy.c
YACC_OUT_C = parser.tab.c
YACC_OUT_H = parser.tab.h

.PHONY: all clean

# Cible par défaut
all: $(TARGET)

# Génération de l'exécutable
$(TARGET): $(YACC_OUT_C) $(LEX_OUT)
	$(CC) $(CFLAGS) $(YACC_OUT_C) $(LEX_OUT) -lfl -o $(TARGET)

# Génération des fichiers du parser
$(YACC_OUT_C) $(YACC_OUT_H): $(YACC_FILE)
	$(YACC) -d $(YACC_FILE)

# Génération du lexer
$(LEX_OUT): $(LEX_FILE)
	$(LEX) $(LEX_FILE)

# Nettoyage
clean:
	rm -f $(TARGET) $(LEX_OUT) $(YACC_OUT_C) $(YACC_OUT_H)

