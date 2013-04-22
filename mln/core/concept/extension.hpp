#ifndef MLN_CORE_CONCEPT_EXTENSION_HPP
# define MLN_CORE_CONCEPT_EXTENSION_HPP

# include <mln/core/concept/object.hpp>
# include <mln/core/extension/extension_traits.hpp>

namespace mln
{

  template <typename E>
  struct Extension
  {
    typedef extension_traits<E> traits;

    typedef typename traits::support_fill       support_fill;
    typedef typename traits::support_mirror     support_mirror;
    typedef typename traits::support_periodize  support_periodize;

    BOOST_CONCEPT_USAGE(Extension)
    {

      MLN_CONCEPT_BEGIN_CHECK_IF()
        {
          typedef typename E::value_type    value_type;
          void (E::*ptr) (const value_type&) = &E::fill;
          (void) ptr;
        }
      MLN_CONCEPT_END_CHECK_IF((support_fill::value));

      MLN_CONCEPT_BEGIN_CHECK_IF()
        {
          void (E::*ptr) () = &E::mirror;
          (void) ptr;
        }
      MLN_CONCEPT_END_CHECK_IF((support_mirror::value));

      MLN_CONCEPT_BEGIN_CHECK_IF()
        {
          void (E::*ptr) () = &E::periodize;
          (void) ptr;
        }
      MLN_CONCEPT_END_CHECK_IF((support_periodize::value));

    }

  };

} // end of namespace mln

#endif //!MLN_CORE_CONCEPT_EXTENSION_HPP
