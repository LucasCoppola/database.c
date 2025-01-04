CC = gcc
CFLAGS = -Wall -Wextra -I./include

all: simple-db test-db

simple-db: src/main.c src/database.c include/database.h
	$(CC) $(CFLAGS) src/main.c src/database.c -o simple-db

test-db: tests/test_database.c src/database.c include/database.h
	$(CC) $(CFLAGS) tests/test_database.c src/database.c -o test-db

test: test-db
	./test-db

clean:
	rm -f simple-db test-db
