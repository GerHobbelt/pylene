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

    constexpr const V& value(const point_type&) const { return m_val; }

    void fill(const value_type& v)
    {
      if (!is_fill_supported())
        throw std::logic_error("Attempting to use fill on an extension that is not fillable!");

      m_val = v;
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
    }
    constexpr bool is_fill_supported() const { return support_fill::value; }
    constexpr bool is_mirror_supported() const { return support_mirror::value; }
    constexpr bool is_periodize_supported() const { return support_periodize::value; }
    constexpr bool is_clamp_supported() const { return support_clamp::value; }
    constexpr bool is_buffer_supported() const { return support_buffer::value; }

  private:
    V m_val;
  };

} // namespace mln::extension
