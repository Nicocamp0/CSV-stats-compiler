CC = gcc
CFLAGS = -Wall -Wextra -g
LEX = flex
YACC = bison
TARGET = analyseur

all: $(TARGET)

parser.tab.c parser.tab.h: parser.y
	$(YACC) -d parser.y

lex.yy.c: lexer.l parser.tab.h
	$(LEX) lexer.l

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

symbol_table.o: symbol_table.c symbol_table.h
	$(CC) $(CFLAGS) -c symbol_table.c

$(TARGET): parser.tab.c lex.yy.c ast.o symbol_table.o
	$(CC) $(CFLAGS) parser.tab.c lex.yy.c ast.o symbol_table.o -lfl -o $(TARGET)

clean:
	rm -f *.o parser.tab.* lex.yy.c $(TARGET)

