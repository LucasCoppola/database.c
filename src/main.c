#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/database.h"
#include "core/table.h"

#include "parser/ast.h"
#include "parser/parser.h"
#include "parser/tokenizer.h"

#include "executor/executor.h"
#include "meta_commands.h"
#include "utils/logger.h"

#define MAX_INPUT_LENGTH 256

void read_input(char *input, size_t input_size) {
  if (fgets(input, input_size, stdin) == NULL) {
    return;
  }

  input[strcspn(input, "\n")] = 0; // Remove newline character
};

int main(int argc, char *argv[]) {
  Database *db = NULL;
  const char *filename = argc > 1 ? argv[1] : NULL;

  if (filename == NULL) {
    printf("Error: No filename specified.\n");
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  DatabaseResult db_result = database_open(&db, filename);
  if (db_result != DATABASE_SUCCESS) {
    LOG_ERROR("database", db_result);
    return 1;
  }

  char input[MAX_INPUT_LENGTH];

  printf("Connected to %s database.\n", filename);
  printf("Type '.help' for commands.\n");

  while (1) {
    printf("db> ");
    read_input(input, sizeof(input));

    if (input[0] == '.') {
      process_meta_command(input, db);
      continue;
    }

    TokenizerState *state = NULL;
    TokenizerResult tokenizer_result = tokenizer_init(input, &state);
    if (tokenizer_result != TOKENIZER_SUCCESS) {
      fprintf(stderr, "Failed to initialize tokenizer.\n");
      continue;
    }

    tokenize_query(state);
    // printf("Tokens:\n");
    // for (int i = 0; i < state->token_count; i++) {
    //   printf("%s\n", state->tokens[i].value);
    // }

    ASTNode *ast_node = parse(state->tokens, state->token_count);
    if (ast_node == NULL) {
      printf("Error: Parsing failed.\n");
      tokenizer_free(state);
      continue;
    }

    ExecuteResult execute_result = execute_ast(db, ast_node);
    switch (execute_result) {
    case EXECUTE_SUCCESS:
      printf("Executed.\n");
      break;
    case EXECUTE_TABLE_FULL:
      printf("Error: Table full.\n");
      break;
    case EXECUTE_FAILURE:
      printf("Error: Execution failed.\n");
      break;
    }

    ast_free(ast_node);
    tokenizer_free(state);
  }

  database_free(db);
  return 0;
}
