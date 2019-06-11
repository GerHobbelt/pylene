#pragma once

#include <mln/core/concept/new/domains.hpp>
#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/image/image.hpp>

#include <algorithm>
#include <cmath>
#include <optional>
#include <type_traits>
#include <utility>


namespace mln::extension
{
  template <typename V, typename I, bool Mutable = true>
  struct by_mirror
  {
    using value_type          = V;
    using point_type          = image_point_t<I>;
    using support_fill        = std::false_type;
    using support_mirror      = std::true_type;
    using support_periodize   = std::false_type;
    using support_clamp       = std::false_type;
    using support_extend_with = std::false_type;

  private:
    using padding_ptr_t = std::conditional_t<Mutable, std::size_t*, const std::size_t*>;

  public:
    explicit by_mirror(padding_ptr_t padding_ptr)
      : m_padding_ptr{padding_ptr}
    {
    }

    std::size_t padding() const { return *m_padding_ptr; }

    template <typename SE>
    constexpr bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      return true;
    }

    constexpr bool is_finite() const { return false; }

    constexpr std::optional<std::size_t> size() const { return std::nullopt; }

    void fill(const value_type& /*v*/)
    {
      if (!is_fill_supported())
        throw std::logic_error("Attempting to use fill on an extension that is not fillable!");
    }
    void mirror(std::size_t padding = 0)
    {
      if (!is_mirror_supported())
        throw std::logic_error("Attempting to use mirror on an extension that is not mirrorable!");

      *m_padding_ptr = padding;

      // Nothing else to do, everything is lazy-computed
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
    void extend_with(U&& /*u*/, point_type /*offset*/ = {})
    {
      if (!is_extend_with_supported())
        throw std::logic_error("Attempting to use extend_with on an extension that is not extendable!");
    }
    constexpr bool is_fill_supported() const { return support_fill::value; }
    constexpr bool is_mirror_supported() const { return support_mirror::value; }
    constexpr bool is_periodize_supported() const { return support_periodize::value; }
    constexpr bool is_clamp_supported() const { return support_clamp::value; }
    constexpr bool is_extend_with_supported() const { return support_extend_with::value; }

  private:
    padding_ptr_t m_padding_ptr;
  };

} // namespace mln::extension
