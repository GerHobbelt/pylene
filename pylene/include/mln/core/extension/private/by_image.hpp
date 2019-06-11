#pragma once

#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/image/image.hpp>

#include <functional>
#include <optional>
#include <type_traits>
#include <utility>


namespace mln::extension
{

  template <typename V, typename P, bool Mutable = true>
  struct by_image
  {
    using value_type        = V;
    using point_type        = P;
    using support_fill      = std::false_type;
    using support_mirror    = std::false_type;
    using support_periodize = std::false_type;
    using support_clamp     = std::false_type;
    using support_extend_with    = std::true_type;

  private:
    using has_value_func_ptr_t = std::conditional_t<Mutable, std::function<bool(const point_type&)>*,
                                                    const std::function<bool(const point_type&)>*>;
    using get_value_func_ptr_t = std::conditional_t<Mutable, std::function<const value_type&(const point_type&)>*,
                                                    const std::function<const value_type&(const point_type&)>*>;

  public:
    by_image(has_value_func_ptr_t f, get_value_func_ptr_t g)
      : m_has_value_func_ptr{f}
      , m_get_value_func_ptr{g}
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
    void extend_with(U&& base_ima, point_type offset = {})
    {
      if (!is_extend_with_supported())
        throw std::logic_error("Attempting to use extend_with on an extension that is not extendable!");

      *m_has_value_func_ptr = [base_ima, offset](const point_type& pnt) { return base_ima.domain().has(pnt + offset); };
      *m_get_value_func_ptr = [base_ima, offset](const point_type& pnt) { return base_ima(pnt + offset); };
    }
    constexpr bool is_fill_supported() const { return support_fill::value; }
    constexpr bool is_mirror_supported() const { return support_mirror::value; }
    constexpr bool is_periodize_supported() const { return support_periodize::value; }
    constexpr bool is_clamp_supported() const { return support_clamp::value; }
    constexpr bool is_extend_with_supported() const { return support_extend_with::value; }

  private:
    has_value_func_ptr_t m_has_value_func_ptr;
    get_value_func_ptr_t m_get_value_func_ptr;
  };

} // namespace mln::extension
