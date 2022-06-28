#include "utils.h"
#include "test.h"

TEST_F(equation, add_symbol)
{
#define TEST_EQ_ADD_SYMBOL(STR, SYMBOL, EXPECTED)               \
  ({                                                            \
    struct equation eq;                                         \
    uint32_t sz = sizeof(STR) - 1;                              \
    memset(&eq, 0, sizeof(eq));                                 \
    utils_str_to_eq(STR, &eq, sz);                              \
    bool ret = equation_add_symbol(&eq, SYMBOL, sz);            \
    EXPECT_TRUE(ret == EXPECTED);                               \
  })

  TEST_EQ_ADD_SYMBOL("1=1", SYMBOL_EQ, false);
  TEST_EQ_ADD_SYMBOL("1", SYMBOL_PLUS, true);
  TEST_EQ_ADD_SYMBOL("1+", SYMBOL_2, true);
  TEST_EQ_ADD_SYMBOL("1+2", SYMBOL_EQ, true);
  TEST_EQ_ADD_SYMBOL("1+2=", SYMBOL_3, true);
  TEST_EQ_ADD_SYMBOL("1+", SYMBOL_MULT, false);

#undef TEST_EQ_ADD_SYMBOL
  return true;
}

TEST_F(equation, check_equality)
{
#define TEST_CHECK_EQUALITY(STR, EXPECTED)      \
  ({                                            \
    struct equation eq;                         \
    uint32_t sz = sizeof(STR) - 1;              \
    memset(&eq, 0, sizeof(eq));                 \
    eq.sz = sz;                                 \
    utils_str_to_eq(STR, &eq, sz);              \
    bool ret = equation_check_equality(&eq);    \
    EXPECT_TRUE(ret == EXPECTED);               \
  })

  TEST_CHECK_EQUALITY("10+2=12", true);
  TEST_CHECK_EQUALITY("1+2=4", false);
  TEST_CHECK_EQUALITY("12*10=120", true);
  TEST_CHECK_EQUALITY("12/6+2=4", true);

#undef TEST_CHECK_EQUALITY
  return true;
}

TEST_F(equation, check_semantic)
{
  static enum symbol s1[] = { 1, SYMBOL_PLUS, 2, 3, SYMBOL_EQ, 2, SYMBOL_PLUS };
  static enum symbol s2[] = { 1, SYMBOL_PLUS, 2, 3, SYMBOL_EQ, 2 };
  static enum symbol s3[] = { 1, SYMBOL_EQ, 1 };

#define TEST_CHECK_SEMANTIC(SYMBOLS, EXPECTED)                  \
  ({                                                            \
    struct equation eq;                                         \
    eq.sz = sizeof(SYMBOLS) / sizeof(enum symbol);              \
    memcpy(eq.symbols, SYMBOLS, sizeof(SYMBOLS));               \
    bool ret = equation_check_semantic(&eq);                    \
    EXPECT_TRUE(ret == EXPECTED);                               \
  })

  TEST_CHECK_SEMANTIC(s1, false);
  TEST_CHECK_SEMANTIC(s2, true);
  TEST_CHECK_SEMANTIC(s3, false);

  return true;
}

const static struct test equation_tests[] = {
  TEST(equation, add_symbol),
  TEST(equation, check_semantic),
  TEST(equation, check_equality),
};

TEST_SUITE(equation);
