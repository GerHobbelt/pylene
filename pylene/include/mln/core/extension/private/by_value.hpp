#pragma once

#include <mln/core/concept/new/structuring_elements.hpp>

#include <type_traits>
#include <utility>


namespace mln::extension
{

  template <typename V, typename P>
  struct by_value
  {
    using value_type        = V;
    using point_type        = P;
    using support_fill      = std::true_type;
    using support_mirror    = std::false_type;
    using support_periodize = std::false_type;
    using support_clamp     = std::false_type;
    using is_finite         = std::false_type;

    explicit by_value(V val)
      : m_val(std::move(val))
    {
    }

    template <typename SE>
    bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      return true;
    }

    void     fill(V val) { m_val = std::move(val); }
    const V& value(const point_type&) const { return m_val; }

  private:
    V m_val;
  };

} // namespace mln::extension
