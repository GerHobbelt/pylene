#pragma once

#include <ano/core/config.hpp>

#include <string_view>

#define ano_concat(A, B) A##B
#define ano_entering_expand(NAME, COUNTER) ano::trace::scoped_trace ano_concat(__ano_trace, COUNTER)(NAME);
#define ano_entering(NAME) ano_entering_expand(NAME, __COUNTER__)
#define ano_exiting() ;
#define ano_scoped_entering(NAME) ano_entering_expand(NAME, __COUNTER__)

namespace ano::trace
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

} // namespace ano::trace

/******************************************/
/****          Implementation          ****/
/******************************************/

namespace ano::trace
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
} // namespace ano::trace
