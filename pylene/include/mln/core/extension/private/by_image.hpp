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
    explicit by_image(U base_ima, point_type offset = point_type{})
      : m_hasvalue{[base_ima, offset](const P& pnt) { return base_ima.domain().has(pnt + offset); }}
      , m_yieldvalue{[base_ima, offset](const P& pnt) { return base_ima(pnt + offset); }}
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

    void fill(const value_type& /*v*/)
    {
      if (!is_fill_supported())
        throw std::logic_error("Attempting to use fill on an extension that is not fillable!");
    }
    void mirror(std::size_t /*padding*/ = 0)
    {
      if (!is_mirror_supported())
        throw std::logic_error("Attempting to use mirror on an extension that is not mirrorable!");
    }
    void periodize()
    {
      if (!is_periodize_supported())
        throw std::logic_error("Attempting to use periodize on an extension that is not periodizable!");
    }
    void clamp()
    {
      if (!is_clamp_supported())
        throw std::logic_error("Attempting to use clamp on an extension that is not clampable!");
    }
    template <typename U>
    void buffer(U&& /*u*/)
    {
      if (!is_buffer_supported())
        throw std::logic_error("Attempting to use buffer on an extension that is not buffurable!");

      // Nothing to do, everything is lazy-computed
    }
    constexpr bool is_fill_supported() const { return support_fill::value; }
    constexpr bool is_mirror_supported() const { return support_mirror::value; }
    constexpr bool is_periodize_supported() const { return support_periodize::value; }
    constexpr bool is_clamp_supported() const { return support_clamp::value; }
    constexpr bool is_buffer_supported() const { return support_buffer::value; }

  private:
    std::function<bool(const P&)>     m_hasvalue;
    std::function<const V&(const P&)> m_yieldvalue;
  };

} // namespace mln::extension
