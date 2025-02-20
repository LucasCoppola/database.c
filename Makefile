CC = gcc
CFLAGS = -g -Wall -Wextra -I./include

# Directories
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
STORAGE_DIR = $(SRC_DIR)/storage
PARSER_DIR = $(SRC_DIR)/parser
EXECUTOR_DIR = $(SRC_DIR)/executor
UTILS_DIR = $(SRC_DIR)/utils
TOKENIZER_DIR = $(PARSER_DIR)/tokenizer
PARSER_STATEMENTS_DIR = $(PARSER_DIR)/statements
EXECUTOR_STATEMENTS_DIR = $(EXECUTOR_DIR)/statements

TESTS_DIR = tests
UNIT_DIR = $(TESTS_DIR)/unit
INTEGRATION_DIR = $(TESTS_DIR)/integration

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
CORE_FILES = $(wildcard $(CORE_DIR)/*.c) $(wildcard $(CORE_DIR)/hashmap/*.c)
STORAGE_FILES = $(wildcard $(STORAGE_DIR)/*.c)
PARSER_FILES = $(wildcard $(PARSER_DIR)/*.c)
EXECUTOR_FILES = $(wildcard $(EXECUTOR_DIR)/*.c)
UTILS_FILES = $(wildcard $(UTILS_DIR)/*.c)
TOKENIZER_FILES = $(wildcard $(TOKENIZER_DIR)/*.c)
PARSER_STATEMENTS_FILES = $(wildcard $(PARSER_STATEMENTS_DIR)/*.c)
EXECUTOR_STATEMENTS_FILES = $(wildcard $(EXECUTOR_STATEMENTS_DIR)/*.c)

# Test files
TEST_UTILS = $(UNIT_DIR)/utils/test_utils.c
TEST_HASHMAP = $(UNIT_DIR)/core/test_hashmap.c
TEST_PARSER = $(UNIT_DIR)/parser/test_parser.c
TEST_TOKENIZER = $(UNIT_DIR)/parser/test_tokenizer.c
TEST_CREATE_TABLE = $(INTEGRATION_DIR)/test_create_table.c
TEST_DROP_TABLE = $(INTEGRATION_DIR)/test_drop_table.c
TEST_PARSER_STATEMENTS = $(wildcard $(UNIT_DIR)/parser/statements/*.c)

# Executables
MAIN_EXEC = main
TEST_HASHMAP_EXEC = test_hashmap
TEST_PARSER_EXEC = test_parser
TEST_TOKENIZER_EXEC = test_tokenizer
TEST_CREATE_TABLE_EXEC = test_create_table
TEST_DROP_TABLE_EXEC = test_drop_table

# Main target
main: $(SRC_FILES) $(CORE_FILES) $(STORAGE_FILES) $(PARSER_FILES) $(EXECUTOR_FILES) $(UTILS_FILES) $(TOKENIZER_FILES) $(PARSER_STATEMENTS_FILES) $(EXECUTOR_STATEMENTS_FILES)
	$(CC) $(CFLAGS) $^ -o $(MAIN_EXEC)

# Test targets
test_hashmap: $(TEST_HASHMAP) $(CORE_FILES) $(STORAGE_FILES) $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_HASHMAP_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_HASHMAP_EXEC)

test_parser: $(TEST_UTILS) $(TEST_PARSER) $(PARSER_FILES) $(TOKENIZER_FILES) $(PARSER_STATEMENTS_FILES) $(CORE_FILES) $(STORAGE_FILES) $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_PARSER_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_PARSER_EXEC)

test_tokenizer: $(TEST_TOKENIZER) $(TOKENIZER_FILES) $(CORE_FILES) $(STORAGE_FILES) $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_TOKENIZER_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_TOKENIZER_EXEC)

test_create_table: $(TEST_CREATE_TABLE) $(TEST_UTILS) $(CORE_FILES) $(STORAGE_FILES) $(PARSER_FILES) $(TOKENIZER_FILES) $(PARSER_STATEMENTS_FILES) $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_CREATE_TABLE_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_CREATE_TABLE_EXEC)

test_drop_table: $(TEST_DROP_TABLE) $(TEST_UTILS) $(CORE_FILES) $(STORAGE_FILES) $(PARSER_FILES) $(TOKENIZER_FILES) $(EXECUTOR_STATEMENTS_FILES) $(PARSER_STATEMENTS_FILES) $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_DROP_TABLE_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_DROP_TABLE_EXEC)

# Run all tests
test: test_hashmap test_parser test_tokenizer test_create_table test_drop_table

# Clean build artifacts
clean:
	rm -f $(MAIN_EXEC) $(TEST_HASHMAP_EXEC) $(TEST_PARSER_EXEC) $(TEST_TOKENIZER_EXEC) $(TEST_CREATE_TABLE_EXEC) $(TEST_DROP_TABLE_EXEC)

.PHONY: main test_hashmap test_tokenizer test_parser test_create_table test clean
