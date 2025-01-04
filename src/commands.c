#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/commands.h"
#include "../include/database.h"

void create_table_cmd(Database *db, char *command) {
  char table_name[256];
  if (sscanf(command, "CREATE TABLE %s", table_name) == 1) {
    create_table(db, table_name);
    printf("Table '%s' created successfully.\n", table_name);
  } else {
    printf("Invalid CREATE TABLE syntax.\n");
  }
}

void select_rows_cmd(Database *db, char *command) {
  char table_name[256], name[256];
  int age;
  sscanf(command, "INSERT INTO %s (name, age) VALUES (%s, %d)", table_name,
         name, &age);
  Table *table = find_table(db, table_name);
  if (table) {
    insert_row(table, name, age);
  } else {
    printf("Table %s does not exist\n", table_name);
  }
}

void insert_rows_cmd(Database *db, char *command) {
  char table_name[256];
  sscanf(command, "SELECT * FROM %s", table_name);
  Table *table = find_table(db, table_name);
  if (table) {
    select_from_table(table);
  } else {
    printf("Table %s does not exist\n", table_name);
  }
}

void delete_rows_cmd(Database *db, char *command) {
  char table_name[256];
  sscanf(command, "SELECT * FROM %s", table_name);
  Table *table = find_table(db, table_name);
  if (table) {
    select_from_table(table);
  } else {
    printf("Table %s does not exist\n", table_name);
  }
}

void help() {
  printf("Available commands:\n");
  printf("CREATE TABLE <table_name>\n");
  printf("SELECT * FROM <table_name>\n");
  printf("INSERT INTO <table_name> (name, age) VALUES (<name>, <age>)\n");
  printf("DELETE FROM <table_name>\n");
  printf("help\n");
  printf("exit\n");
}

void process_command(Database *db, char *command) {
  if (strncmp(command, "CREATE TABLE", 12) == 0) {
    create_table_cmd(db, command);
  } else if (strncmp(command, "INSERT INTO", 11) == 0) {
    insert_rows_cmd(db, command);
  } else if (strncmp(command, "SELECT * FROM", 13) == 0) {
    select_rows_cmd(db, command);
  } else if (strncmp(command, "DELETE FROM", 11) == 0) {
    delete_rows_cmd(db, command);
  } else if (strcasecmp(command, CMD_HELP) == 0) {
    help();
  } else if (strcasecmp(command, CMD_EXIT) == 0) {
    exit(0);
  } else {
    printf("Unknown command: %s\n", command);
    help();
  }
}
