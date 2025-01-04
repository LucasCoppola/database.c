#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/commands.h"
#include "../include/database.h"

int main() {
  Database *db = create_database();

  char command[256];
  printf("Connected to a transient in-memory database.\n");
  printf("Type 'help' for commands.\n");

  while (1) {
    printf("simple-db> ");
    if (fgets(command, sizeof(command), stdin) == NULL) {
      break;
    }

    command[strcspn(command, "\n")] = 0; // Remove newline character

    if (strcasecmp(command, CMD_HELP) == 0 ||
        strcasecmp(command, CMD_EXIT) == 0) {
      process_command(db, command);
    } else {
      char *token = strtok(command, ";");
      while (token != NULL) {
        process_command(db, token);
        token = strtok(NULL, ";");
      }
    }
  }

  free_database(db);
  return 0;
}
