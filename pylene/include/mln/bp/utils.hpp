#pragma once
#include <cassert>
#include <cstddef>

namespace mln::bp
{

  template <class T>
  [[gnu::always_inline]] inline T* ptr_offset(T* in, std::ptrdiff_t offset)
  {
    return (T*)((char*)in + offset);
  }
}


#ifndef __has_builtin         // Optional of course.
#define __has_builtin(x) 0  // Compatibility with non-clang compilers.
#endif

#ifdef _MSC_VER
#define GSL_ASSUME(cond) __assume(cond)
#elif defined(__clang__)
#define GSL_ASSUME(cond) __builtin_assume(cond)
#elif defined(__GNUC__)
#define GSL_ASSUME(cond) ((cond) ? static_cast<void>(0) : __builtin_unreachable())
#else
#define GSL_ASSUME(cond) static_cast<void>(!!(cond))
#endif


#ifndef NDEBUG
#define mln_assume(x) assert(x)
#else
#define mln_assume(x) GSL_ASSUME(x)
#endif
