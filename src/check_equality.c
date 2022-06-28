#include <assert.h>
#include <stdlib.h>

#include "equation.h"

/**
 * Type of a node.
 */
enum node_type {
  NODE_OPERATOR,
  NODE_OPERAND,
};

/**
 * Element of the list.
 */
struct node {
  enum node_type type;
  uint32_t value;
  struct node *next;
  struct node *prev;
};

/**
 * Structure used to reduce an equation and check the equality.
 */
struct list {
  struct node *head;
  struct node *tail;
  uint32_t nr; /* number of nodes */
};

/**
 * Create a new node.
 */
static struct node* node_new(enum node_type type, uint32_t value)
{
  struct node *node = calloc(1, sizeof(*node));
  node->type = type;
  node->value = value;
  return node;
}

/**
 * Insert a node at the tail of the list.
 */
static void list_insert_tail(struct list *list, struct node *node)
{
  if (list->head == NULL) {
    list->head = node;
  }
  if (list->tail != NULL) {
    list->tail->next = node;
    node->prev = list->tail;
  }
  list->tail = node;
}

/**
 * Add a new element to the list.
 */
static void list_add(struct list *list, enum node_type type, uint32_t value)
{
  struct node *node = node_new(type, value);
  list_insert_tail(list, node);
  ++list->nr;
}

/**
 * Clean the list.
 */
static void list_free(struct list *list)
{
  struct node *current = list->head;

  while (current != NULL) {
    struct node *to_free = current;
    current = current->next;
    free(to_free);
  }
  free(list);
}

#define SYMBOL_IS_OPERAND(SYMBOL) \
  (SYMBOL >= SYMBOL_0 && SYMBOL <= SYMBOL_9)

/**
 * An an operand node to the list.
 * Operand can be defined by multiple smbols.
 */
static uint32_t list_add_operand(struct equation *eq,
                                 struct list *list,
                                 uint32_t i)
{
  enum symbol symbol = eq->symbols[i];
  uint32_t value = 0;

  do {
    value *= 10;
    value += symbol;
    if (i + 1 == eq->sz ||
        SYMBOL_IS_OPERAND(eq->symbols[i + 1]) == false) {
      break;
    }
    symbol = eq->symbols[++i];
  } while (true);

  list_add(list, NODE_OPERAND, value);
  return i;
}

/**
 * Convert an equation to a list of nodes.
 */
static struct list* list_convert(struct equation *eq)
{
  struct list *list = calloc(1, sizeof(*list));

  for (uint32_t i = 0; i < eq->sz; ++i) {
    if (SYMBOL_IS_OPERAND(eq->symbols[i])) {
      i = list_add_operand(eq, list, i);
    } else {
      list_add(list, NODE_OPERATOR, eq->symbols[i]);
    }
  }
  return list;
}

/**
 * + Div by 0 is forbidden.
 * + Only allows integer division.
 */
static bool check_div(struct node *node,
                      struct node *left,
                      struct node *right)
{
  if (node->value != SYMBOL_DIV) {
    return true;
  }
  if (right->value == 0) {
    return false;
  }
  if ((left->value / right->value) * right->value != left->value) {
    return false;
  }
  return true;
}

/**
 * Calculate the operation.
 */
static uint32_t calculate(enum symbol operator, uint32_t v1, uint32_t v2)
{
  switch (operator) {
    case SYMBOL_DIV:
      return v1 / v2;
    case SYMBOL_MULT:
      return v1 * v2;
    case SYMBOL_MINUS:
      return v1 - v2;
    case SYMBOL_PLUS:
      return v1 + v2;
    default:
      ;
  };
  assert(!"cannot calculate unknown operator");
}

/**
 * Join the previous and next nodes to the midle one.
 * from: N_X <--> N_left <--> N_midle <--> N_right <--> N_Y>
 * to: N_X <--> N_midle <--> N_Y
 */
static void nodes_join(struct list *list,
                       struct node *left,
                       struct node *midle,
                       struct node *right)
{
  midle->prev = left->prev;
  midle->next = right->next;

  if (left->prev != NULL) {
    left->prev->next = midle;
  } else {
    list->head = midle;
  }

  if (right->next != NULL) {
    right->next->prev = midle;
  } else {
    list->tail = midle;
  }
}

/**
 * Merge three nodes of type:
 *  + left: operand
 *  + right: operand
 *  + operator
 */
static bool nodes_merge(struct list *list,
                        struct node *left,
                        struct node *operator,
                        struct node *right)
{
  if (check_div(operator, left, right) == false) {
    return false;
  }

  nodes_join(list, left, operator, right);

  operator->value = calculate(operator->value, left->value, right->value);
  operator->type = NODE_OPERAND;

  free(left);
  free(right);
  list->nr -= 2;
  return true;
}

/**
 * Reduce a specified operator.
 *
 * + div: 6/2 -> 3
 * + plus: 12+2 -> 14
 * + ...
 */
static bool reduce_operator(struct list *list, enum symbol operator)
{
  struct node *node = list->head;

  while (node != NULL) {
    if (node->type == NODE_OPERATOR && node->value == operator) {
      struct node *left = node->prev;
      struct node *right = node->next;
      if (left == NULL || left->type != NODE_OPERAND ||
          right == NULL || right->type != NODE_OPERAND) {
        return false;
      }
      if (nodes_merge(list, left, node, right) == false) {
        return false;
      }
    }
    node = node->next;
  }
  return true;
}

/**
 * Reduce an equation to a form: <operand = operand>.
 *
 *  + 12+6/2=42+1 -> 15=43
 *  + 1+1+1=1+1+1 -> 3=3
 *
 *  1. reduce '/'
 *  2. reduce '*'
 *  3. reduce '-'
 *  4. reduce '+'
 */
static bool reduce(struct list *list)
{
  if (reduce_operator(list, SYMBOL_DIV) == false) {
    return false;
  }
  if (reduce_operator(list, SYMBOL_MULT) == false) {
    return false;
  }
  if (reduce_operator(list, SYMBOL_MINUS) == false) {
    return false;
  }
  if (reduce_operator(list, SYMBOL_PLUS) == false) {
    return false;
  }
  return true;
}

/**
 * Check validity of an equation of type: <operand = operand>.
 */
static bool check_result(struct list *list)
{
  if (list->nr != 3) {
    return false;
  }

  struct node *left = list->head; /* left operand */
  struct node *right = list->tail; /* right operand */
  struct node *operator = list->head->next;

  /* check types */
  if (left->type != NODE_OPERAND ||
      right->type != NODE_OPERAND ||
      operator->type != NODE_OPERATOR) {
    return false;
  }

  /* check result */
  if (operator->value != SYMBOL_EQ || left->value != right->value) {
    return false;
  }

  return true;
}

bool equation_check_equality(struct equation *eq)
{
  struct list *list = list_convert(eq);
  reduce(list);
  bool ret = check_result(list);
  list_free(list);
  return ret;
}
