CC = gcc
CFLAGS = -g -Wall -Wextra -I./include

SRC_DIR = src
DATABASE_DIR = $(SRC_DIR)/database
HASHMAP_DIR = $(SRC_DIR)/hashmap
STORAGE_DIR = $(SRC_DIR)/storage
PARSER_DIR = $(SRC_DIR)/parser
EXECUTOR_DIR = $(SRC_DIR)/executor

TOKENIZER_DIR = $(PARSER_DIR)/tokenizer
PARSER_STATEMENTS_DIR = $(PARSER_DIR)/statements
EXECUTOR_STATEMENTS_DIR = $(EXECUTOR_DIR)/statements

TESTS_DIR = tests

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
DATABASE_FILES = $(wildcard $(DATABASE_DIR)/*.c)
HASHMAP_FILES = $(wildcard $(HASHMAP_DIR)/*.c)
STORAGE_FILES = $(wildcard $(STORAGE_DIR)/*.c)
PARSER_FILES = $(wildcard $(PARSER_DIR)/*.c)
EXECUTOR_FILES = $(wildcard $(EXECUTOR_DIR)/*.c)
TOKENIZER_FILES = $(wildcard $(TOKENIZER_DIR)/*.c)
PARSER_STATEMENTS_FILES = $(wildcard $(PARSER_STATEMENTS_DIR)/*.c)
EXECUTOR_STATEMENTS_FILES = $(wildcard $(EXECUTOR_STATEMENTS_DIR)/*.c)

TEST_UTILS = $(TESTS_DIR)/test_utils.c
TEST_HASHMAP = $(TESTS_DIR)/test_hashmap.c
TEST_PARSER = $(TESTS_DIR)/test_parser/test_parser.c
TEST_TOKENIZER = $(TESTS_DIR)/test_tokenizer.c

MAIN_EXEC = main
TEST_HASHMAP_EXEC = test_hashmap
TEST_PARSER_EXEC = test_parser
TEST_TOKENIZER_EXEC = test_tokenizer

main: $(SRC_FILES) $(DATABASE_FILES) $(HASHMAP_FILES) $(STORAGE_FILES) $(PARSER_FILES) $(EXECUTOR_FILES) $(TOKENIZER_FILES) $(PARSER_STATEMENTS_FILES) $(EXECUTOR_STATEMENTS_FILES)
	$(CC) $(CFLAGS) $^ -o $(MAIN_EXEC)

test_hashmap: $(TEST_HASHMAP) $(HASHMAP_FILES) $(DATABASE_FILES) $(SRC_FILES) $(STORAGE_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_HASHMAP_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_HASHMAP_EXEC)

test_parser: $(TEST_UTILS) $(TEST_PARSER) $(PARSER_FILES) $(TOKENIZER_FILES) $(STATEMENTS_FILES) $(HASHMAP_FILES) $(DATABASE_FILES) $(SRC_FILES) $(STORAGE_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_PARSER_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_PARSER_EXEC)

test_tokenizer: $(TEST_TOKENIZER) $(TOKENIZER_FILES) $(HASHMAP_FILES) $(DATABASE_FILES) $(SRC_FILES) $(STORAGE_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_TOKENIZER_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_TOKENIZER_EXEC)

test: test_hashmap test_parser test_tokenizer

clean:
	rm -f $(MAIN_EXEC) $(TEST_HASHMAP_EXEC) $(TEST_PARSER_EXEC) $(TEST_TOKENIZER_EXEC)

.PHONY: main test_hashmap test_tokenizer test_parser test clean
