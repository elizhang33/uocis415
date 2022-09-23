CFLAGS = -W -Wall
OBJECTS = part3.o token_func.o

part3: $(OBJECTS)
			gcc $(LDFLAGS) -o part3 $(OBJECTS)

part3.o: part3.c
token_func.o: token_func.c

clean:
			rm -f $(OBJECTS) part3
