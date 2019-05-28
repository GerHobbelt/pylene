#pragma once

#include <mln/core/concept/new/structuring_elements.hpp>

#include <optional>
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
    using support_buffer    = std::false_type;

    explicit by_value(V val)
      : m_val(std::move(val))
    {
    }

    template <typename SE>
    constexpr bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      return true;
    }

    constexpr bool is_finite() const { return false; }

    constexpr std::optional<std::size_t> size() const { return std::nullopt; }

    constexpr void     fill(V val) { m_val = std::move(val); }
    constexpr const V& value(const point_type&) const { return m_val; }

  private:
    V m_val;
  };

} // namespace mln::extension
