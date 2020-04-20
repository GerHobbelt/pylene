#ifndef MLN_CORE_FOREACH_HPP
#define MLN_CORE_FOREACH_HPP

#include <mln/core/iterator/iterator_utils.hpp>
#include <mln/core/range/iter.hpp>

#include <utility>

namespace mln
{

  namespace internal
  {
    template <typename T>
    struct false_var_t
    {
      false_var_t(T&& v) : x_{std::forward<T>(v)} {}

      constexpr operator bool() const { return false; }
      T& get() { return x_; }
      false_var_t& set(const T& v)
      {
        x_ = v;
        return *this;
      }

      T x_;
    };

    template <class T>
    false_var_t<T> make_local_variable(T&& expr)
    {
      return false_var_t<T>(std::forward<T>(expr));
    }
  }
}

#define MLN_DECL_VAR(ID, VALUE)                                                                                        \
  if (auto ID = mln::internal::make_local_variable(VALUE))                                                             \
  {                                                                                                                    \
  }                                                                                                                    \
  else

/******************************************/
/****         mln_foreach macro         ****/
/******************************************/

namespace mln
{
  namespace internal
  {
    template <class T>
    T& iter_outer_init(T& x)
    {
      mln::outer_init(x);
      return x;
    }
  }
}

#define mln_foreach(p, COL)                                                                                            \
  MLN_DECL_VAR(_mln_range_, COL)                                                                                       \
  MLN_DECL_VAR(_mln_it_proxy_, mln::rng::iter(_mln_range_.get()))                                                      \
  MLN_DECL_VAR(__mln_has_been_broken, false)                                                                           \
  for (auto& _mln_it_ = mln::internal::iter_outer_init(_mln_it_proxy_.get());                                          \
       !__mln_has_been_broken.get() && !mln::outer_finished(_mln_it_);                                                 \
       __mln_has_been_broken.get() ? (void)0 : mln::outer_next(_mln_it_))                                              \
    for (mln::inner_init(_mln_it_); !__mln_has_been_broken.get() && !mln::inner_finished(_mln_it_);                    \
         __mln_has_been_broken.get() ? (void)0 : mln::inner_next(_mln_it_))                                            \
      if (__mln_has_been_broken.set(true))                                                                             \
      {                                                                                                                \
      }                                                                                                                \
      else                                                                                                             \
        for (p = *_mln_it_; __mln_has_been_broken.get(); __mln_has_been_broken.set(false))

#define mln_reverse_foreach(p, COL)                                                                                    \
  MLN_DECL_VAR(_mln_range_, COL)                                                                                       \
  MLN_DECL_VAR(_mln_it_, mln::rng::riter(_mln_range_.get()))                                                           \
  MLN_DECL_VAR(_mln_continue_, true)                                                                                   \
  for (_mln_it_.get().init(); _mln_continue_.get() and !_mln_it_.get().finished();                                     \
       _mln_continue_.get() ? _mln_it_.get().next() : (void)0)                                                         \
    if (_mln_continue_.set(false))                                                                                     \
    {                                                                                                                  \
    }                                                                                                                  \
    else                                                                                                               \
      for (p = *(_mln_it_.get()); !_mln_continue_.get(); _mln_continue_.set(true))

#endif // ! MLN_CORE_FOREACH_HPP
