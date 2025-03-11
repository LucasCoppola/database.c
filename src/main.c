#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/database.h"
#include "core/table.h"

#include "libs/linenoise.h"
#include "parser/ast.h"
#include "parser/parser.h"
#include "parser/semantic_analyzer.h"
#include "parser/tokenizer.h"

#include "executor/executor.h"
#include "meta_commands.h"
#include "utils/logger.h"

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
    DEBUG_LOG("database", "open", db_result);
    return 1;
  }

  printf("Connected to %s database.\n", filename);
  printf("Type '.help' to list all available commands.\n");

  linenoiseHistoryLoad(HISTORY_FILE);
  linenoiseHistorySetMaxLen(MAX_HISTORY_LENGTH);

  while (1) {
    char *input = linenoise("db> ");
    if (!input)
      break;

    if (input[0] == '\0') {
      linenoiseFree(input);
      continue;
    }

    if (input[0] != '\0') {
      linenoiseHistoryAdd(input);
    }

    if (input[0] == '.') {
      process_meta_command(input, db);
      linenoiseFree(input);
      continue;
    }

    TokenizerState *state = NULL;
    TokenizerResult tokenizer_init_result = tokenizer_init(input, &state);
    if (tokenizer_init_result != TOKENIZER_SUCCESS) {
      DEBUG_LOG("tokenizer", "init", tokenizer_init_result);
      linenoiseFree(input);
      continue;
    }

    TokenizerResult tokenize_query_result = tokenize_query(state);
    if (tokenize_query_result != TOKENIZER_SUCCESS) {
      if (tokenize_query_result == TOKENIZER_UNTERMINATED_STATEMENT) {
        fprintf(stderr,
                "Syntax Error: Unterminated statement at position %d.\n",
                state->position);
      } else {
        DEBUG_LOG("tokenizer", "tokenize_query", tokenize_query_result);
      }
      tokenizer_free(state);
      linenoiseFree(input);
      continue;
    }

    ASTNode *ast_node = parse(state->tokens, state->token_count);
    if (ast_node == NULL) {
      tokenizer_free(state);
      linenoiseFree(input);
      continue;
    }

    SemanticResult semantic_result = semantic_analyze(db, ast_node);
    if (semantic_result != SEMANTIC_SUCCESS) {
      tokenizer_free(state);
      ast_free(ast_node);
      linenoiseFree(input);
      continue;
    }

    ExecuteResult execute_result = execute_ast(db, ast_node);
    switch (execute_result) {
    case EXECUTE_SUCCESS:
      printf("Executed.\n");
      break;
    case EXECUTE_FAILURE:
      printf("Execution failed.\n");
      break;
    }

    ast_free(ast_node);
    tokenizer_free(state);
    linenoiseFree(input);
  }

  linenoiseHistorySave(HISTORY_FILE);
  database_close(db);
  return 0;
}
