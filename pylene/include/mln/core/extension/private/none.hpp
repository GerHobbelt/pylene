#pragma once

#include <mln/core/concept/new/structuring_elements.hpp>

#include <type_traits>
#include <utility>


namespace mln::extension
{

  template <typename V, typename P>
  struct none
  {
    using value_type        = V;
    using point_type        = P;
    using support_fill      = std::false_type;
    using support_mirror    = std::false_type;
    using support_periodize = std::false_type;
    using support_clamp     = std::false_type;
    using is_finite         = std::true_type;

    none() {}

    template <typename SE>
    bool fit(const SE& se) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

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

    const value_type& value(const point_type&) const { throw std::runtime_error(" Accessing point out of bound !"); }
  };

} // namespace mln::extension
