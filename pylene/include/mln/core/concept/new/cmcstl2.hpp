#pragma once

/* DO NOT REMOVE */

// stl2/meta and ranges-v3/meta are not the same but have the same header guard (><)!!.
// stl2 works with ranges-v3/meta, but ranges-v3 doesn't work with stl2/meta
// here we force the usage of ranges-v3/meta
// stl2/meta is in <stl2/meta/meta.hpp>
#include <meta/meta.hpp>


#ifdef PYLENE_CONCEPT_TS_ENABLED

#ifdef concept
#define __concept
#undef concept
#endif

#include <stl2/concepts.hpp>

#ifdef __concept
#define concept concept bool
#undef __concept
#endif

namespace mln::concepts::stl
{
  using namespace __stl2;
}


// Dirty hack to allow proper conversion from ranges iterator iterator tags to __stl2 iterator tags
#include <range/v3/utility/iterator_concepts.hpp>

#include <type_traits>

namespace std::experimental::ranges
{
  inline namespace v1
  {
    namespace detail
    {
      template <typename T>
      struct rangesv3_to_stl2_iterator_category;

      template <>
      struct rangesv3_to_stl2_iterator_category<::ranges::input_iterator_tag>
      {
        using type = input_iterator_tag;
      };

      template <>
      struct rangesv3_to_stl2_iterator_category<::ranges::forward_iterator_tag>
      {
        using type = forward_iterator_tag;
      };

      template <>
      struct rangesv3_to_stl2_iterator_category<::ranges::bidirectional_iterator_tag>
      {
        using type = bidirectional_iterator_tag;
      };

      template <>
      struct rangesv3_to_stl2_iterator_category<::ranges::random_access_iterator_tag>
      {
        using type = random_access_iterator_tag;
      };

      template <typename T>
      using rangesv3_to_stl2_iterator_category_t = typename rangesv3_to_stl2_iterator_category<T>::type;
    } // namespace detail

    template <detail::MemberIteratorCategory T>
    requires DerivedFrom<typename T::iterator_category, ::ranges::input_iterator_tag> struct iterator_category<T>
    {
      using type = detail::rangesv3_to_stl2_iterator_category_t<typename T::iterator_category>;
    };

  } // namespace v1
} // namespace std::experimental::ranges

#endif // PYLENE_CONCEPT_TS_ENABLED
