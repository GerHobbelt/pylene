#pragma once

#include <mln/core/concept/new/extensions.hpp>

#include <mln/core/concept/check.hpp>
#include <mln/core/concept/object.hpp>
#include <mln/core/extension/extension_traits.hpp>


namespace mln
{

  namespace internal
  {

    template <class E>
    struct FillableExtension
    {
      typedef extension_traits<E>           traits;
      typedef typename traits::support_fill support_fill;

      BOOST_CONCEPT_USAGE(FillableExtension)
      {
        typedef typename E::value_type value_type;
        void (E::*ptr)(const value_type&) = &E::fill;
        (void)ptr;

        check(support_fill());
      }
    };

    template <class E>
    struct MirrorableExtension
    {
      typedef extension_traits<E>             traits;
      typedef typename traits::support_mirror support_mirror;

      BOOST_CONCEPT_USAGE(MirrorableExtension)
      {
        void (E::*ptr)(std::size_t) = &E::mirror;
        (void)ptr;

        check(support_mirror());
      }
    };

    template <class E>
    struct PeriodizableExtension
    {
      typedef extension_traits<E>                traits;
      typedef typename traits::support_periodize support_periodize;

      BOOST_CONCEPT_USAGE(PeriodizableExtension)
      {
        void (E::*ptr)() = &E::periodize;
        (void)ptr;
        check(support_periodize());
      }
    };
  } // namespace internal

  namespace internal
  {

    template <class E, bool enable = extension_traits<E>::support_fill::value>
    struct check_fill
    {
      static void check() {}
    };

    template <class E>
    struct check_fill<E, true>
    {
      static void check() { BOOST_CONCEPT_ASSERT((FillableExtension<E>)); }
    };

    template <class E, bool enable = extension_traits<E>::support_periodize::value>
    struct check_periodize
    {
      static void check() {}
    };

    template <class E>
    struct check_periodize<E, true>
    {
      static void check() { BOOST_CONCEPT_ASSERT((PeriodizableExtension<E>)); }
    };

    template <class E, bool enable = extension_traits<E>::support_mirror::value>
    struct check_mirror
    {
      static void check() {}
    };

    template <class E>
    struct check_mirror<E, true>
    {
      static void check() { BOOST_CONCEPT_ASSERT((MirrorableExtension<E>)); }
    };
  } // namespace internal

  template <typename E>
  struct Extension
  {
    typedef extension_traits<E> traits;

    typedef typename traits::support_fill      support_fill;
    typedef typename traits::support_mirror    support_mirror;
    typedef typename traits::support_periodize support_periodize;

  private:
  public:
    BOOST_CONCEPT_USAGE(Extension)
    {
      internal::check_fill<E>::check();
      internal::check_periodize<E>::check();
      internal::check_mirror<E>::check();
    }
  };

} // namespace mln
