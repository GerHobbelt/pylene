#pragma once

#include <mln/core/concepts/domain.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/private/traits/extension.hpp>

#include <limits>
#include <type_traits>
#include <utility>


namespace mln::extension
{
  struct by_mirror
  {
    using support_fill        = std::false_type;
    using support_mirror      = std::true_type;
    using support_periodize   = std::false_type;
    using support_clamp       = std::false_type;
    using support_extend_with = std::false_type;

    template <typename SE>
    constexpr bool fit(const SE&) const
    {
      static_assert(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      return true;
    }

    constexpr int extent() const { return std::numeric_limits<int>::max(); }

    void mirror() noexcept {}

    constexpr bool is_fill_supported() const { return support_fill::value; }
    constexpr bool is_mirror_supported() const { return support_mirror::value; }
    constexpr bool is_periodize_supported() const { return support_periodize::value; }
    constexpr bool is_clamp_supported() const { return support_clamp::value; }
    constexpr bool is_extend_with_supported() const { return support_extend_with::value; }
  };

} // namespace mln::extension
