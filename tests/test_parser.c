#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/parser.h"
#include "../include/tokenizer.h"

DataType parse_data_type(const char *type) {
  if (strcmp(type, "INT") == 0)
    return TYPE_INT;
  if (strcmp(type, "TEXT") == 0)
    return TYPE_TEXT;
  return -1;
}

char *parse_node_type(NodeType type) {
  switch (type) {
  case NODE_CREATE_TABLE:
    return "NODE_CREATE_TABLE";
    break;
  case NODE_DROP_TABLE:
    return "NODE_DROP_TABLE";
    break;
  case NODE_SELECT:
    return "NODE_SELECT";
    break;
  case NODE_INSERT:
    return "NODE_INSERT";
    break;
  case NODE_DELETE:
    return "NODE_DELETE";
    break;
  default:
    return "UNKNOWN";
  }
}

void print_ast_node(const ASTNode *node) {
  if (!node) {
    printf("ASTNode: NULL\n");
    return;
  }

  char *node_type_str = parse_node_type(node->type);
  printf("  ASTNode:\n");
  printf("    Type: %s\n", node_type_str);
  printf("    Table Name: %s\n", node->table_name);
  printf("    Columns:\n");
  for (int i = 0; i < node->create_table.num_columns; i++) {
    const char *type_str = (node->create_table.columns[i].type == TYPE_INT)
                               ? "TYPE_INT"
                               : "TYPE_TEXT";
    printf("      %s: %s\n", node->create_table.columns[i].name, type_str);
  }
}

void test_drop_table(const char *query, bool should_pass,
                     const char *expected_table_name) {
  printf("Testing query: %s\n", query);

  TokenizerState *state = NULL;
  TokenizerResult result = tokenizer_init(query, &state);
  if (result != TOKENIZER_SUCCESS) {
    fprintf(stderr, "Failed to initialize tokenizer.\n");
    return;
  }
  tokenize_query(state);

  ASTNode *node = parser_table_drop(state->tokens);

  if (should_pass) {
    if (!node) {
      printf("  FAIL: Expected parsing to succeed, but it failed.\n");
    } else {
      if (strcmp(node->table_name, expected_table_name) != 0) {
        printf("  FAIL: Expected table name '%s', got '%s'.\n",
               expected_table_name, node->table_name);
      } else {
        printf(
            "  PASS: Parsing succeeded and output matches expected values.\n");
      }
    }
  } else {
    if (node) {
      printf("  FAIL: Expected parsing to fail, but it succeeded.\n");
    } else {
      printf("  PASS: Parsing failed as expected.\n");
    }
  }

  if (node) {
    free(node);
  }
  tokenizer_free(state);
  printf("\n");
}

void test_create_table(const char *query, bool should_pass,
                       const char *expected_table_name,
                       const char **expected_columns,
                       int expected_num_columns) {
  printf("Testing query: %s\n", query);

  TokenizerState *state = NULL;
  TokenizerResult result = tokenizer_init(query, &state);
  if (result != TOKENIZER_SUCCESS) {
    fprintf(stderr, "Failed to initialize tokenizer.\n");
    return;
  }
  tokenize_query(state);

  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  if (should_pass) {
    if (!node) {
      printf("  FAIL: Expected parsing to succeed, but it failed.\n");
    } else {
      if (strcmp(node->table_name, expected_table_name) != 0) {
        printf("  FAIL: Expected table name '%s', got '%s'.\n",
               expected_table_name, node->table_name);
      } else if (node->create_table.num_columns != expected_num_columns) {
        printf("  FAIL: Expected %d columns, got %d.\n", expected_num_columns,
               node->create_table.num_columns);
      } else {
        bool columns_match = true;
        for (int i = 0; i < expected_num_columns; i++) {
          const char *expected_name = expected_columns[2 * i];
          DataType expected_type = parse_data_type(expected_columns[2 * i + 1]);
          if (strcmp(node->create_table.columns[i].name, expected_name) != 0 ||
              node->create_table.columns[i].type != expected_type) {
            columns_match = false;
            break;
          }
        }
        if (!columns_match) {
          printf("  FAIL: Columns do not match expected values.\n");
        } else {
          printf("  PASS: Parsing succeeded and output matches expected "
                 "values.\n");
          print_ast_node(node);
        }
      }
    }
  } else {
    if (node) {
      printf("  FAIL: Expected parsing to fail, but it succeeded.\n");
      print_ast_node(node);
    } else {
      printf("  PASS: Parsing failed as expected.\n");
    }
  }
  if (node) {
    free(node->create_table.columns);
    free(node);
  }
  tokenizer_free(state);
  printf("\n");
}

int main() {
  const char *query1 = "CREATE TABLE users (id INT, name TEXT);";
  const char *expected_columns1[] = {"id", "INT", "name", "TEXT"};
  test_create_table(query1, true, "users", expected_columns1, 2);

  const char *query2 =
      "CREATE TABLE products (product_id INT, product_name TEXT, price INT);";
  const char *expected_columns2[] = {"product_id", "INT",   "product_name",
                                     "TEXT",       "price", "INT"};
  test_create_table(query2, true, "products", expected_columns2, 3);

  const char *query3 = "CREATE TABLE invalid (id INT, name);";
  test_create_table(query3, false, NULL, NULL, 0);

  const char *query4 = "CREATE TABLE missing_paren (id INT, name TEXT";
  test_create_table(query4, false, NULL, NULL, 0);

  const char *query5 = "CREATE TABLE empty ();";
  test_create_table(query5, false, NULL, NULL, 0);

  const char *query6 = "DROP TABLE users;";
  test_drop_table(query6, true, "users");

  const char *query7 = "DROP TABLE;";
  test_drop_table(query7, false, NULL);

  const char *query8 = "DROP users;";
  test_drop_table(query8, false, NULL);

  return 0;
}
