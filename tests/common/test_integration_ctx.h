#ifndef TEST_INTEGRATION_CONTEXT_H
#define TEST_INTEGRATION_CONTEXT_H

#include "parser/tokenizer.h"
#include "parser/ast.h"
#include "core/database.h"
#include "test_utils.h"

typedef struct TestIntegrationCtx {
    Database *db;
    TokenizerState *state;
    ASTNode *node;
} TestIntegrationCtx;

static inline void test_integration_ctx_init(TestIntegrationCtx *ctx, const char *db_name) {
    ctx->db = NULL;
    ctx->state = NULL;
    ctx->node = NULL;

    DatabaseResult db_result = database_open(&ctx->db, db_name);
    TEST_ASSERT_EQUAL(DATABASE_SUCCESS, db_result);
    TEST_ASSERT_NOT_NULL(ctx->db);
}

static inline void test_integration_ctx_teardown(TestIntegrationCtx *ctx, const char *db_name) {
    if (ctx->node) {
        ast_free(ctx->node);
        ctx->node = NULL;
    }
    if (ctx->state) {
        tokenizer_free(ctx->state);
        ctx->state = NULL;
    }
    if (ctx->db) {
        database_close(ctx->db);
        ctx->db = NULL;
    }
    remove(db_name);
}

#endif
