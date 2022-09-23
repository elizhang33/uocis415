CFLAGS = -W -Wall
OBJECTS = part5.o token_func.o

part5: $(OBJECTS)
			gcc $(LDFLAGS) -o part5 $(OBJECTS)

part5.o: part5.c
token_func.o: token_func.c

clean:
			rm -f $(OBJECTS) part5
