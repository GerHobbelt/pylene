#pragma once

#include <mln/core/config.hpp>

#include <string_view>

#define mln_concat(A, B) A##B
#define mln_entering_expand(NAME, COUNTER) mln::trace::scoped_trace mln_concat(__mln_trace, COUNTER)(NAME);
#define mln_entering(NAME) mln_entering_expand(NAME, __COUNTER__)
#define mln_exiting() ;
#define mln_scoped_entering(NAME) mln_entering_expand(NAME, __COUNTER__)

namespace mln::trace
{
  extern bool verbose;

  struct scoped_trace
  {
    scoped_trace(std::string_view desc);
    ~scoped_trace();

  private:
    struct impl_t;

    [[gnu::noinline]] void entering(std::string_view desc);
    [[gnu::noinline]] void exiting();

    impl_t* m_impl = nullptr;
  };

  namespace impl
  {
    [[gnu::noinline]] void warn(std::string_view msg);
  }

} // namespace mln::trace

/******************************************/
/****          Implementation          ****/
/******************************************/

namespace mln::trace
{
  inline scoped_trace::scoped_trace(std::string_view desc)
  {
    if (verbose)
      entering(desc);
  }


  inline scoped_trace::~scoped_trace()
  {
    if (m_impl)
      exiting();
  }

  inline void warn(std::string_view msg)
  {
    if (verbose)
      impl::warn(msg);
  }
} // namespace mln::trace
