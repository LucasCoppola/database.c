CC = gcc
CFLAGS = -g -Wall -Wextra -I./include

# Directories
SRC_DIR = src
LIBS_DIR = libs
CORE_DIR = $(SRC_DIR)/core
STORAGE_DIR = $(SRC_DIR)/storage
PARSER_DIR = $(SRC_DIR)/parser
EXECUTOR_DIR = $(SRC_DIR)/executor
UTILS_DIR = $(SRC_DIR)/utils
TOKENIZER_DIR = $(PARSER_DIR)/tokenizer
PARSER_STATEMENTS_DIR = $(PARSER_DIR)/statements
SEMANTIC_ANALYZER_DIR = $(PARSER_DIR)/semantic_analyzer
SEMANTIC_ANALYZER_VALIDATORS_DIR = $(SEMANTIC_ANALYZER_DIR)/validators

TESTS_DIR = tests
UNIT_DIR = $(TESTS_DIR)/unit
INTEGRATION_DIR = $(TESTS_DIR)/integration
COMMON_DIR = $(TESTS_DIR)/common

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
CORE_FILES = $(wildcard $(CORE_DIR)/*.c) $(wildcard $(CORE_DIR)/hashmap/*.c)
STORAGE_FILES = $(wildcard $(STORAGE_DIR)/*.c)
PARSER_FILES = $(wildcard $(PARSER_DIR)/*.c)
PARSER_STATEMENTS_FILES = $(wildcard $(PARSER_STATEMENTS_DIR)/*.c)
EXECUTOR_FILES = $(wildcard $(EXECUTOR_DIR)/*.c)
UTILS_FILES = $(wildcard $(UTILS_DIR)/*.c)
TOKENIZER_FILES = $(wildcard $(TOKENIZER_DIR)/*.c)
SEMANTIC_ANALYZER_FILES = $(wildcard $(SEMANTIC_ANALYZER_DIR)/*.c)
SEMANTIC_ANALYZER_VALIDATORS_FILES = $(wildcard $(SEMANTIC_ANALYZER_VALIDATORS_DIR)/*.c)

LINENOISE_LIB = $(wildcard $(LIBS_DIR)/linenoise.c)
UNITY_LIB = $(wildcard $(LIBS_DIR)/unity.c)

# Test files
TEST_COMMON = $(COMMON_DIR)/test_utils.c
TEST_HASHMAP = $(UNIT_DIR)/core/test_hashmap.c
TEST_TOKENIZER = $(UNIT_DIR)/parser/test_tokenizer.c

TEST_PARSER_CREATE_TABLE = $(UNIT_DIR)/parser/statements/test_create_table.c
TEST_PARSER_DROP_TABLE = $(UNIT_DIR)/parser/statements/test_drop_table.c
TEST_PARSER_INSERT = $(UNIT_DIR)/parser/statements/test_insert.c
TEST_PARSER_SELECT = $(UNIT_DIR)/parser/statements/test_select.c

TEST_CREATE_TABLE = $(INTEGRATION_DIR)/test_create_table.c
TEST_DROP_TABLE = $(INTEGRATION_DIR)/test_drop_table.c
TEST_INSERT_ROW = $(INTEGRATION_DIR)/test_insert_row.c

# Executables
MAIN_EXEC = main
TEST_HASHMAP_EXEC = test_hashmap
TEST_TOKENIZER_EXEC = test_tokenizer

TEST_PARSER_CREATE_TABLE_EXEC = test_parser_create_table
TEST_PARSER_DROP_TABLE_EXEC = test_parser_drop_table
TEST_PARSER_INSERT_EXEC = test_parser_insert
TEST_PARSER_SELECT_EXEC = test_parser_select

TEST_CREATE_TABLE_EXEC = test_create_table
TEST_DROP_TABLE_EXEC = test_drop_table
TEST_INSERT_ROW_EXEC = test_insert_row

# Main target
main: $(SRC_FILES) $(LINENOISE_LIB) $(CORE_FILES) $(STORAGE_FILES) \
    $(PARSER_FILES) $(EXECUTOR_FILES) $(UTILS_FILES) $(TOKENIZER_FILES) \
    $(PARSER_STATEMENTS_FILES) $(EXECUTOR_FILES) \
    $(SEMANTIC_ANALYZER_FILES) $(SEMANTIC_ANALYZER_VALIDATORS_FILES)
	$(CC) $(CFLAGS) $^ -o $(MAIN_EXEC)

# Test targets
test_hashmap: $(UNITY_LIB) $(TEST_HASHMAP) $(CORE_FILES) $(STORAGE_FILES) \
    $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_HASHMAP_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_HASHMAP_EXEC)
	rm -f ./$(TEST_HASHMAP_EXEC)

test_tokenizer: $(UNITY_LIB) $(TEST_TOKENIZER) $(TOKENIZER_FILES) \
    $(CORE_FILES) $(STORAGE_FILES) $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_TOKENIZER_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_TOKENIZER_EXEC)
	rm -f ./$(TEST_TOKENIZER_EXEC)

test_parser_create_table: $(UNITY_LIB) $(TEST_COMMON) \
    $(TEST_PARSER_CREATE_TABLE) $(PARSER_FILES) $(PARSER_STATEMENTS_FILES) \
    $(TOKENIZER_FILES) $(CORE_FILES) $(STORAGE_FILES) $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_PARSER_CREATE_TABLE_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_PARSER_CREATE_TABLE_EXEC)
	rm -f ./$(TEST_PARSER_CREATE_TABLE_EXEC)

test_parser_insert: $(UNITY_LIB) $(TEST_COMMON) $(TEST_PARSER_INSERT) \
    $(PARSER_FILES) $(PARSER_STATEMENTS_FILES) $(TOKENIZER_FILES) \
    $(CORE_FILES) $(STORAGE_FILES) $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_PARSER_INSERT_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_PARSER_INSERT_EXEC)
	rm -f ./$(TEST_PARSER_INSERT_EXEC)

test_parser_select: $(UNITY_LIB) $(TEST_COMMON) $(TEST_PARSER_SELECT) \
    $(PARSER_FILES) $(PARSER_STATEMENTS_FILES) $(TOKENIZER_FILES) \
    $(CORE_FILES) $(STORAGE_FILES) $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_PARSER_SELECT_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_PARSER_SELECT_EXEC)
	rm -f ./$(TEST_PARSER_SELECT_EXEC)

test_parser_drop_table: $(UNITY_LIB) $(TEST_COMMON) \
    $(TEST_PARSER_DROP_TABLE) $(PARSER_FILES) $(PARSER_STATEMENTS_FILES) \
    $(TOKENIZER_FILES) $(CORE_FILES) $(STORAGE_FILES) $(UTILS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_PARSER_DROP_TABLE_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_PARSER_DROP_TABLE_EXEC)
	rm -f ./$(TEST_PARSER_DROP_TABLE_EXEC)

test_create_table: $(UNITY_LIB) $(TEST_CREATE_TABLE) $(TEST_COMMON) \
    $(CORE_FILES) $(STORAGE_FILES) $(PARSER_FILES) $(TOKENIZER_FILES) \
    $(PARSER_STATEMENTS_FILES) $(UTILS_FILES) $(SEMANTIC_ANALYZER_FILES) \
    $(SEMANTIC_ANALYZER_VALIDATORS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_CREATE_TABLE_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_CREATE_TABLE_EXEC)
	rm -f ./$(TEST_CREATE_TABLE_EXEC)

test_drop_table: $(UNITY_LIB) $(TEST_DROP_TABLE) $(TEST_COMMON) \
    $(CORE_FILES) $(STORAGE_FILES) $(PARSER_FILES) $(TOKENIZER_FILES) \
    $(EXECUTOR_FILES) $(PARSER_STATEMENTS_FILES) $(UTILS_FILES) \
    $(SEMANTIC_ANALYZER_FILES) $(SEMANTIC_ANALYZER_VALIDATORS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_DROP_TABLE_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_DROP_TABLE_EXEC)
	rm -f ./$(TEST_DROP_TABLE_EXEC)

test_insert_row: $(UNITY_LIB) $(TEST_INSERT_ROW) $(TEST_COMMON) \
    $(CORE_FILES) $(STORAGE_FILES) $(PARSER_FILES) $(TOKENIZER_FILES) \
    $(EXECUTOR_FILES) $(PARSER_STATEMENTS_FILES) $(UTILS_FILES) \
    $(SEMANTIC_ANALYZER_FILES) $(SEMANTIC_ANALYZER_VALIDATORS_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST_INSERT_ROW_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_INSERT_ROW_EXEC)
	rm -f ./$(TEST_INSERT_ROW_EXEC)

test_integration: test_create_table test_drop_table test_insert_row

test_parser: test_parser_create_table test_parser_drop_table \
    test_parser_insert test_parser_select

test: test_hashmap test_tokenizer test_parser_create_table \
    test_parser_drop_table test_parser_insert test_parser_select \
    test_create_table test_drop_table test_insert_row

clean:
	rm -f $(MAIN_EXEC) $(TEST_HASHMAP_EXEC) $(TEST_TOKENIZER_EXEC) \
    $(TEST_PARSER_CREATE_TABLE_EXEC) $(TEST_PARSER_DROP_TABLE_EXEC) \
    $(TEST_PARSER_INSERT_EXEC) $(TEST_PARSER_SELECT_EXEC) \
    $(TEST_CREATE_TABLE_EXEC) $(TEST_DROP_TABLE_EXEC) \
    $(TEST_INSERT_ROW_EXEC)

.PHONY: main test_hashmap test_tokenizer test_parser test_create_table \
    test_drop_table test_insert_row test clean \
    test_parser_create_table test_parser_drop_table test_parser_insert \
    test_parser_select
