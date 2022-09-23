CFLAGS = -W -Wall
OBJECTS = part4.o token_func.o

part4: $(OBJECTS)
			gcc $(LDFLAGS) -o part4 $(OBJECTS)

part4.o: part4.c
token_func.o: token_func.c

clean:
			rm -f $(OBJECTS) part4
