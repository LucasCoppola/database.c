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
      Statement statement;
      switch (prepare_statement(command, &statement)) {
      case (PREPARE_SUCCESS):
        break;
      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("Error: in prepare, near '%s': syntax error\n", command);
        continue;
      }

      execute_statement(db, &statement);
      printf("Executed.\n");
    }
  }

  free_database(db);
  return 0;
}
