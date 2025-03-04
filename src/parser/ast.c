#include <stdbool.h>
#include <stdlib.h>

#include "core/table.h"
#include "parser/ast.h"

#include "core/row.h"

ASTNodeResult create_ast_node(NodeType type, ASTNode **out_node) {
  ASTNode *node = malloc(sizeof(ASTNode));
  if (!node) {
    return AST_ALLOC_ERROR;
  }

  node->type = type;
  *out_node = node;
  return AST_SUCCESS;
}

void ast_free(ASTNode *node) {
  if (!node)
    return;

  switch (node->type) {
  case NODE_CREATE_TABLE:
    if (node->create_table.columns) {
      for (int i = 0; i < node->create_table.num_columns; i++) {
        if (node->create_table.columns[i].name) {
          free(node->create_table.columns[i].name);
        }
      }
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

  case NODE_INSERT:
    if (node->insert_rows.values) {
      for (int i = 0; i < node->insert_rows.num_values; i++) {
        if (node->insert_rows.values[i].type == COLUMN_TYPE_TEXT &&
            node->insert_rows.values[i].string_value) {
          free(node->insert_rows.values[i].string_value);
        }
      }
      free(node->insert_rows.values);
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
