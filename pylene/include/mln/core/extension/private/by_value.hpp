#pragma once

#include <mln/core/concepts/structuring_element.hpp>


#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace mln::extension
{
  template <typename V>
  struct by_value
  {
    using value_type          = std::remove_cvref_t<V>;
    using support_fill        = std::true_type;
    using support_mirror      = std::false_type;
    using support_periodize   = std::false_type;
    using support_clamp       = std::false_type;
    using support_extend_with = std::false_type;

  public:
    explicit by_value(value_type* valptr)
      : m_valptr(valptr)
    {
    }

    template <typename SE>
    constexpr bool fit(const SE&) const
    {
      static_assert(mln::is_a_v<SE, details::StructuringElement>, "SE is not a valid Structuring Element!");

      return true;
    }

    constexpr int extent() const { return std::numeric_limits<int>::max(); }

    void fill(const value_type& v) noexcept { *m_valptr = v; }

    constexpr bool is_fill_supported() const { return support_fill::value; }
    constexpr bool is_mirror_supported() const { return support_mirror::value; }
    constexpr bool is_periodize_supported() const { return support_periodize::value; }
    constexpr bool is_clamp_supported() const { return support_clamp::value; }
    constexpr bool is_extend_with_supported() const { return support_extend_with::value; }

  private:
    value_type* m_valptr; // mutable
  };

} // namespace mln::extension
