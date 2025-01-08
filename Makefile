CC = gcc
CFLAGS = -g -Wall -I./include

main:
	$(CC) $(CFLAGS) src/*.c src/storage/*.c -o main 

test:
	$(CC) $(CFLAGS) tests/test_hashmap.c src/hashmap.c src/hashmap_utils.c -o test_hashmap
	valgrind --leak-check=full --show-leak-kinds=all ./test_hashmap

clean:
	rm -f main test_hashmap

.PHONY: test clean
