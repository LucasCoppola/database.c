#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "core/database.h"
#include "meta_commands.h"

void help() {
  printf(".tables     List all existing tables in the database.\n");
  printf(".schema     Show table schemas.\n");
  printf(".help       List of all available commands.\n");
  printf(".exit       Exit this program.\n");
}

void process_meta_command(char *command, Database *db) {
  if (strcasecmp(command, META_CMD_HELP) == 0) {
    help();
  } else if (strcasecmp(command, META_CMD_TABLES) == 0) {
    database_tables_list(db);
  } else if (strcasecmp(command, META_CMD_SCHEMA) == 0) {
    database_schema(db);
  } else if (strcasecmp(command, META_CMD_EXIT) == 0) {
    exit(0);
  } else {
    printf("Error: unknown command or invalid arguments: '%s' Enter '.help' "
           "for help.\n",
           command);
  }
}
