
#define MLN_PRIVATE_GEN_UNARY_CODE(FUN, TYPE, OP)                                                                      \
  inline TYPE FUN(TYPE x) { return OP; }

#define MLN_PRIVATE_GEN_BINARY_CODE(FUN, TYPE, OP)                                                                     \
  inline TYPE FUN(TYPE x, TYPE y) { return OP; }

#define MLN_PRIVATE_GEN_TEMPLATED_UNARY_CODE(TEMPLATE, FUN, TYPE, OP)                                                  \
  TEMPLATE inline TYPE FUN(TYPE x) { return OP; }

#define MLN_PRIVATE_GEN_TEMPLATED_BINARY_CODE(TEMPLATE, FUN, TYPE, OP)                                                 \
  TEMPLATE inline TYPE FUN(TYPE x, TYPE y) { return OP; }
