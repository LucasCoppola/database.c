#include <stdbool.h>
#include <stdio.h>

#include "libs/unity.h"
#include "parser/tokenizer.h"
#include "utils/logger.h"

void setUp(void) {}

void tearDown(void) {}

void test_tokenizer_query(const char *query) {
  TokenizerState *state = NULL;
  TokenizerResult init_result = tokenizer_init(query, &state);

  TEST_ASSERT_EQUAL(TOKENIZER_SUCCESS, init_result);

  TokenizerResult result = tokenize_query(state);

  TEST_ASSERT_EQUAL(TOKENIZER_SUCCESS, result);

  tokenizer_free(state);
}

void test_create_table(void) {
  const char *query = "CREATE TABLE users (id INT, username TEXT, email TEXT);";
  test_tokenizer_query(query);
}

void test_insert_into(void) {
  const char *query = "INSERT INTO users (id, username, email) VALUES (1, "
                      "'alice', 'alice@example.com');";
  test_tokenizer_query(query);
}

void test_select_all(void) {
  const char *query = "SELECT * FROM users;";
  test_tokenizer_query(query);
}

void test_select_columns(void) {
  const char *query = "SELECT username, email FROM users;";
  test_tokenizer_query(query);
}

void test_update(void) {
  const char *query =
      "UPDATE users SET email = 'alice@newdomain.com' WHERE id = 1;";
  test_tokenizer_query(query);
}

void test_delete(void) {
  const char *query = "DELETE FROM users WHERE id = 2;";
  test_tokenizer_query(query);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_create_table);
  RUN_TEST(test_insert_into);
  RUN_TEST(test_select_all);
  RUN_TEST(test_select_columns);
  RUN_TEST(test_update);
  RUN_TEST(test_delete);

  return UNITY_END();
}
