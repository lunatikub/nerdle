#include "utils.h"
#include "test.h"

TEST_F(equation, equation_add_symbol)
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

const static struct test equation_tests[] = {
  TEST(equation, equation_add_symbol),
};

TEST_SUITE(equation);
