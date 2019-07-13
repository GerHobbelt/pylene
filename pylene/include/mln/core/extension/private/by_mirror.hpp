#pragma once

#include <mln/core/concept/new/domains.hpp>
#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/image/image.hpp>

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

  public:
    explicit by_mirror(std::size_t* padding_ptr)
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

    constexpr int extent() const { return std::numeric_limits<int>::max(); }

    void mirror(std::size_t padding = 0) noexcept
    {
      *m_padding_ptr = padding;

      // Nothing else to do, everything is lazy-computed
    }

    constexpr bool is_fill_supported() const { return support_fill::value; }
    constexpr bool is_mirror_supported() const { return support_mirror::value; }
    constexpr bool is_periodize_supported() const { return support_periodize::value; }
    constexpr bool is_clamp_supported() const { return support_clamp::value; }
    constexpr bool is_extend_with_supported() const { return support_extend_with::value; }

  private:
    std::size_t* m_padding_ptr; // mutable
  };

} // namespace mln::extension
