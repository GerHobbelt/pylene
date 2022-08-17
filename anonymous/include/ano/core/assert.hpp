#undef ano_assertion
#undef ano_precondition
#undef ano_postcondition
#undef ANO_HAS_DEBUG

#include <cassert>

#ifndef _ANO_HAS_NDEBUG_FIRST_
#if defined(NDEBUG) && !defined(ANO_NDEBUG)
#define ANO_NDEBUG
#endif
#define _ANO_HAS_NDEBUG_FIRST_
#endif

#ifndef ANO_NDEBUG
#define ano_assertion(expr) assert(expr)
#define ANO_HAS_DEBUG 1
#define ANO_EVAL_IF_DEBUG(expr) expr
#else
#define ano_assertion(expr) (void(0))
#define ANO_HAS_DEBUG 0
#define ANO_EVAL_IF_DEBUG(expr)
#endif

#define ano_precondition(expr) ano_assertion(expr)
#define ano_postcondition(expr) ano_assertion(expr)
