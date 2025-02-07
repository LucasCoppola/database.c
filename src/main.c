#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/database.h"
#include "../include/logger.h"
#include "../include/meta_commands.h"
#include "../include/statements.h"

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

    Statement stmt;
    PrepareResult prepare_result = prepare_statement(input, &stmt);

    if (prepare_result == PREPARE_SYNTAX_ERROR) {
      printf("Error: in prepare, near '%s': syntax error\n", input);
      continue;
    }

    if (prepare_result == PREPARE_UNRECOGNIZED_STATEMENT) {
      printf("Error: Unrecognized statement\n");
      continue;
    }

    ExecuteResult result = execute_statement(db, &stmt);
    switch (result) {
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
  }

  database_free(db);
  return 0;
}
