CC = gcc
CFLAGS = -Wall -Wextra -g -IInclude

SRC = Source
INC = Include

all: analyseur

main.o: $(SRC)/main.c
	$(CC) $(CFLAGS) -c $(SRC)/main.c

analyseur: parser.tab.c lex.yy.c main.o ast.o symbol_table.o semantic.o codegen.o
	$(CC) $(CFLAGS) parser.tab.c lex.yy.c main.o ast.o symbol_table.o semantic.o codegen.o -lfl -o analyseur

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

ast.o: $(SRC)/ast.c $(INC)/ast.h
	$(CC) $(CFLAGS) -c $(SRC)/ast.c

symbol_table.o: $(SRC)/symbol_table.c $(INC)/symbol_table.h
	$(CC) $(CFLAGS) -c $(SRC)/symbol_table.c

semantic.o: $(SRC)/semantic.c $(INC)/semantic.h
	$(CC) $(CFLAGS) -c $(SRC)/semantic.c

codegen.o: $(SRC)/codegen.c $(INC)/codegen.h
	$(CC) $(CFLAGS) -c $(SRC)/codegen.c

clean:
	rm -f analyseur parser.tab.c parser.tab.h lex.yy.c *.o afstat.c afstat out.c out __af*
