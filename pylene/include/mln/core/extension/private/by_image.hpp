#pragma once

#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/image/image.hpp>

#include <functional>
#include <optional>
#include <type_traits>
#include <utility>


namespace mln::extension
{

  template <typename V, typename P>
  struct by_image
  {
    using value_type        = V;
    using point_type        = P;
    using support_fill      = std::false_type;
    using support_mirror    = std::false_type;
    using support_periodize = std::false_type;
    using support_clamp     = std::false_type;
    using support_buffer    = std::true_type;

    template <typename U>
    explicit by_image(U base_ima)
      : m_hasvalue{[base_ima](const P& pnt) { return base_ima.domain().has(pnt); }}
      , m_yieldvalue{[base_ima](const P& pnt) { return base_ima(pnt); }}
    {
    }

    template <typename SE>
    constexpr bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");
      // TODO: non-trivial
      return true;
    }

    constexpr bool is_finite() const { return true; }

    constexpr std::optional<std::size_t> size() const
    {
      //  TODO: non trivial
      return 0;
    }

    const V& value(const P& pnt) const
    {
      if (!m_hasvalue(pnt))
        throw std::runtime_error(" Accessing point out of bound !");

      return m_yieldvalue(pnt);
    }

    template <typename U>
    constexpr void buffer(U&&)
    {
      // Nothing to do, everything is lazy-computed
    }

  private:
    std::function<bool(const P&)>     m_hasvalue;
    std::function<const V&(const P&)> m_yieldvalue;
  };

} // namespace mln::extension
