#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../include/parser.h"

ASTNode *create_ast_node(NodeType type) {
  ASTNode *node = malloc(sizeof(ASTNode));
  if (!node) {
    fprintf(stderr, "Failed to allocate new ast node");
    return NULL;
  }

  node->type = type;
  return node;
}

void ast_free(ASTNode *node) {
  if (!node)
    return;

  switch (node->type) {
  case NODE_CREATE_TABLE:
    if (node->create_table.columns) {
      free(node->create_table.columns);
    }
    break;
  case NODE_SELECT:
    if (node->select_rows.select_columns) {
      for (int i = 0; i < node->select_rows.num_columns; i++) {
        if (node->select_rows.select_columns[i]) {
          free(node->select_rows.select_columns[i]);
        }
      }
      free(node->select_rows.select_columns);
    }
    break;
  default:
    break;
  }

  if (node->table_name) {
    free(node->table_name);
  }
  free(node);
}
