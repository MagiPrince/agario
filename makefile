CC = gcc -g -Wall -Wextra -std=gnu11 -fsanitize=address
LIBS = -lpthread -lSDL2 -lrt -lm
OPTS = -Wextra -Wall

agario: agario.o lib/thread.o gfx.o collision.o stack.o worker.o keyboard.o
	$(CC) $^ -o $@ $(LIBS)

threads.o: threads.c
	$(CC) -c $^

agario.o: agario.c agario.h worker.h collision.h keyboard.h lib/gfx.h
	$(CC) -c $^

gfx.o: lib/gfx.c lib/gfx.h
	$(CC) $< -c

collision.o: collision.c lib/gfx.h
	$(CC) -c $^

stack.o: stack.c stack.h
	$(CC) $< -c

worker.o: worker.c worker.h
	$(CC) -c $^

keyboard.o: keyboard.c lib/gfx.h
	$(CC) -c $^

all: clean demo

clean:
	rm *.o lib/*.o *.gch agario

demo: agario
	./agario 960 600 0 0.001 0.01 0.05 0.1 30 4 10