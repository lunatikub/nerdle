#include "utils.h"
#include "test.h"
#include "test_common.h"

static enum symbol s1[] = { 1, P, 2, E, 3 };
static enum symbol s2[] = { 1, 2, T, 4, 9, E, 5, 8, 8 };
static enum symbol s3[] = { M, M, M, M, M, M };

TEST_F(utils, eq_to_str)
{
#define TEST_EQ_TO_STR(SYMBOLS, EXPECTED)                       \
  ({                                                            \
    struct equation eq;                                         \
    char str[LIMIT_MAX_EQ_SZ] = {};                             \
    uint32_t sz = sizeof(SYMBOLS) / sizeof(enum symbol);        \
    memcpy(eq.symbols, SYMBOLS, sizeof(SYMBOLS));               \
    utils_eq_to_str(&eq, str, sz);                              \
    if (strncmp(str, EXPECTED, sz) != 0) {                      \
      return false;                                             \
    }                                                           \
  })

  TEST_EQ_TO_STR(s1, "1+2=3");
  TEST_EQ_TO_STR(s2, "12*49=588");
  TEST_EQ_TO_STR(s3, "------");

  return true;
}

TEST_F(utils, str_to_eq)
{
#define TEST_STR_TO_EQ(STR, EXPECTED)                           \
  ({                                                            \
    struct equation eq;                                         \
    uint32_t sz = sizeof(STR) - 1;                              \
    memset(&eq, 0, sizeof(eq));                                 \
    utils_str_to_eq(STR, &eq, sz);                              \
    if (memcmp(eq.symbols, EXPECTED, sz) != 0) {                \
      return false;                                             \
    }                                                           \
  })

  TEST_STR_TO_EQ("1+2=3", s1);
  TEST_STR_TO_EQ("12*49=588", s2);
  TEST_STR_TO_EQ("------", s3);

  return true;
}

const static struct test utils_tests[] = {
  TEST(utils, eq_to_str),
  TEST(utils, str_to_eq),
};

TEST_SUITE(utils);
