#pragma once

#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/image/image.hpp>

#include <functional>
#include <limits>
#include <type_traits>
#include <utility>


namespace mln::extension
{

  template <typename V, typename P>
  struct by_image
  {
    using value_type          = V;
    using point_type          = P;
    using support_fill        = std::false_type;
    using support_mirror      = std::false_type;
    using support_periodize   = std::false_type;
    using support_clamp       = std::false_type;
    using support_extend_with = std::true_type;

  private:
    using has_value_func_ptr_t = std::function<bool(const point_type&)>*;
    using get_value_func_ptr_t = std::function<const value_type&(const point_type&)>*;

  public:
    by_image(has_value_func_ptr_t f, get_value_func_ptr_t g)
      : m_has_value_func_ptr{f}
      , m_get_value_func_ptr{g}
    {
    }

    template <typename SE>
    constexpr bool fit(const SE&) const
    {
      static_assert(mln::is_a_v<SE, details::StructuringElement>, "SE is not a valid Structuring Element!");

      // TODO: non-trivial
      return true;
    }

    constexpr int extent() const
    {
      //  TODO: non trivial
      return 0;
    }

    template <typename U>
    void extend_with(U&& base_ima, point_type offset = {}) noexcept
    {
      *m_has_value_func_ptr = [base_ima, offset](const point_type& pnt) { return base_ima.domain().has(pnt + offset); };
      *m_get_value_func_ptr = [base_ima, offset](const point_type& pnt) { return base_ima(pnt + offset); };
    }
    constexpr bool is_fill_supported() const { return support_fill::value; }
    constexpr bool is_mirror_supported() const { return support_mirror::value; }
    constexpr bool is_periodize_supported() const { return support_periodize::value; }
    constexpr bool is_clamp_supported() const { return support_clamp::value; }
    constexpr bool is_extend_with_supported() const { return support_extend_with::value; }

  private:
    has_value_func_ptr_t m_has_value_func_ptr; // mutable
    get_value_func_ptr_t m_get_value_func_ptr; // mutable
  };

} // namespace mln::extension
