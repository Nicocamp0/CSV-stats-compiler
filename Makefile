CC = gcc
CFLAGS = -Wall -Wextra -g

all: analyseur

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

symbol_table.o: symbol_table.c symbol_table.h
	$(CC) $(CFLAGS) -c symbol_table.c

semantic.o: semantic.c semantic.h
	$(CC) $(CFLAGS) -c semantic.c

codegen.o: codegen.c codegen.h
	$(CC) $(CFLAGS) -c codegen.c

analyseur: parser.tab.c lex.yy.c ast.o symbol_table.o semantic.o codegen.o
	$(CC) $(CFLAGS) parser.tab.c lex.yy.c ast.o symbol_table.o semantic.o codegen.o -lfl -o analyseur

clean:
	rm -f analyseur parser.tab.c parser.tab.h lex.yy.c *.o afstat.c afstat
