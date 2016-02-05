#ifndef MLN_CORE_CONFIG_HPP

#include <ciso646> // FOR MSVC alternative operator representations

#define BOOST_PP_VARIADICS 1

#ifndef MLN_NO_TBB
# define MLN_HAS_TBB 1
#else
# define MLN_HAS_TBB 0
#endif

#endif // !MLN_CORE_CONFIG_HPP
