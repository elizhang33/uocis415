CFLAGS = -W -Wall -g -pthread -lpthread
OBJECTS = main.o string_parser.o

a.out: $(OBJECTS)
			gcc $(LDFLAGS) -o a.out $(OBJECTS)

main.o: main.c

string_parser.o: string_parser.c

clean:
			rm -f $(OBJECTS) a.out
