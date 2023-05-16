CC = gcc
CFLAGS = -g
LIBRARIES = -ly -lfl
YACC = yacc
YACCFLAGS = -dv -Wcounterexamples
LEX = lex
LEXFLAGS = -l

dragon: dragon.tab.o lex.yy.o stack.o queue.o tree.o hashmap.o scope.o semantic.o codegen.o utils.o
	$(CC) $(CFLAGS) -o dragon dragon.tab.o lex.yy.o tree.o queue.o hashmap.o scope.o semantic.o stack.o codegen.o utils.o $(LIBRARIES)

codegen.o: codegen.c
	$(CC) $(CFLAGS) -Wall -c codegen.c
semantic.o: semantic.c
	$(CC) $(CFLAGS) -Wall -c semantic.c
utils.o: utils.c
	$(CC) $(CFLAGS) -Wall -c utils.c

#structures
tree.o: structures/tree.c
	$(CC) $(CFLAGS) -c structures/tree.c
stack.o: structures/stack.c
	$(CC) $(CFLAGS) -c structures/stack.c
scope.o: scope.c
	$(CC) $(CFLAGS) -c scope.c
hashmap.o: structures/hashmap.c
	$(CC) $(CFLAGS) -c structures/hashmap.c
queue.o: structures/queue.c
	$(CC) $(CFLAGS) -c structures/queue.c

#lex/yacc
dragon.tab.o: dragon.tab.c
	$(CC) $(CFLAGS) -c dragon.tab.c
dragon.tab.c: dragon.y
	$(YACC) $(YACCFLAGS) -b dragon dragon.y

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c
lex.yy.c: dragon.l
	$(LEX) $(LEXFLAGS) dragon.l

clean:
	rm -Rf dragon *.o *.gch structures/*.o structures/*.gch lex.yy.* dragon.tab.* *.output

