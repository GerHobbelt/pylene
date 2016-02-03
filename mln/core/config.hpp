#include <ciso646> // FOR MSVC alternative operator representations

#define BOOST_PP_VARIADICS 1

#ifndef MLN_USE_TBB
# define MLN_HAS_TBB 1
#else
# define MLN_HAS_TBB 0
#endif
