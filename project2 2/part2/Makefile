CFLAGS = -W -Wall
OBJECTS = part2.o token_func.o

a.out: $(OBJECTS)
			gcc $(LDFLAGS) -o part2 $(OBJECTS)

part2.o: part2.c
token_func.o: token_func.c

clean:
			rm -f $(OBJECTS) part2
