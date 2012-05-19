#undef mln_assertion
#undef mln_precondition
#undef mln_postcondition
#undef MLN_HAS_DEBUG

#include <cassert>

#ifndef MLN_NDEBUG
# define mln_assertion(expr) assert(expr)
# define MLN_HAS_DEBUG 1
# 
#else
# define mln_assertion(expr) (void (0))
# define MLN_HAS_DEBUG 0
#endif

#define mln_precondition(expr) mln_assertion(expr)
#define mln_postcondition(expr) mln_assertion(expr)
