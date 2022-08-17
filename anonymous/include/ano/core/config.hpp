#pragma once

#include <ciso646> // FOR MSVC alternative operator representations

#define BOOST_PP_VARIADICS 1

#ifndef ANO_NO_TBB
#define ANO_HAS_TBB 1
#else
#define ANO_HAS_TBB 0
#endif

#if defined(__GNUC__) // Clang or GCC
#define __ano__attribute__(A) __attribute__(A)
#else
#define __ano__attribute__(A)
#endif
