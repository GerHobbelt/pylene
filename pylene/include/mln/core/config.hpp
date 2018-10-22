#ifndef MLN_CORE_CONFIG_HPP

#include <ciso646> // FOR MSVC alternative operator representations

#define BOOST_PP_VARIADICS 1

#ifndef MLN_NO_TBB
#define MLN_HAS_TBB 1
#else
#define MLN_HAS_TBB 0
#endif

#if defined(__GNUC__) // Clang or GCC
#define __mln__attribute__(A) __attribute__(A)
#else
#define __mln__attribute__(A)
#endif

#endif // !MLN_CORE_CONFIG_HPP
