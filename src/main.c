#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/database.h"
#include "../include/meta_commands.h"
#include "../include/sql_statements.h"

int main() {
  Database *db = create_database();
  char command[256];
  printf("Connected to a transient in-memory database.\n");
  printf("Type 'help' for commands.\n");

  while (1) {
    printf("db> ");
    if (fgets(command, sizeof(command), stdin) == NULL) {
      break;
    }
    command[strcspn(command, "\n")] = 0; // Remove newline character

    if (command[0] == '.') {
      process_meta_command(command);
    } else {
      SQLStatement statement;
      PrepareResult table_result = prepare_table_statement(command, &statement);

      if (table_result == PREPARE_SUCCESS) {
        execute_table_statement(db, &statement);
        printf("Executed.\n");
        continue;
      }

      PrepareResult row_result = prepare_row_statement(command, &statement);

      if (row_result == PREPARE_SUCCESS) {
        execute_row_statement(db, &statement);
        printf("Executed.\n");
        continue;
      }

      if (table_result == PREPARE_SYNTAX_ERROR ||
          row_result == PREPARE_SYNTAX_ERROR) {
        printf("Error: in prepare, near '%s': syntax error\n", command);
        continue;
      }

      if (table_result == PREPARE_UNRECOGNIZED_STATEMENT &&
          row_result == PREPARE_UNRECOGNIZED_STATEMENT) {
        printf("Error: unrecognized statement\n");
        continue;
      }
    }
  }

  free_database(db);
  return 0;
}
