#pragma once

#include <mln/core/concept/structuring_elements.hpp>

#include <type_traits>
#include <utility>


namespace mln::extension
{

  struct none
  {
    using support_fill        = std::false_type;
    using support_mirror      = std::false_type;
    using support_periodize   = std::false_type;
    using support_clamp       = std::false_type;
    using support_extend_with = std::false_type;

    none() {}

    template <typename SE>
    constexpr bool fit(const SE& se) const
    {
      static_assert(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      if constexpr (std::is_base_of_v<mln::dynamic_neighborhood_tag, typename SE::category>)
      {
        return se.radial_extent() == 0;
      }
      else
      {
        return false;
      }
      return false;
    }

    constexpr int extent() const { return 0; }

    constexpr bool is_fill_supported() const { return support_fill::value; }
    constexpr bool is_mirror_supported() const { return support_mirror::value; }
    constexpr bool is_periodize_supported() const { return support_periodize::value; }
    constexpr bool is_clamp_supported() const { return support_clamp::value; }
    constexpr bool is_extend_with_supported() const { return support_extend_with::value; }
  };

} // namespace mln::extension
