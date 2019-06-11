#pragma once

#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/utils/remove_cvref.hpp>

#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>


namespace mln::extension
{
  template <typename V, typename P, bool Mutable = true>
  struct by_value
  {
    using value_type          = mln::detail::remove_cvref_t<V>;
    using point_type          = P;
    using support_fill        = std::true_type;
    using support_mirror      = std::false_type;
    using support_periodize   = std::false_type;
    using support_clamp       = std::false_type;
    using support_extend_with = std::false_type;

  private:
    using value_ptr_t = std::conditional_t<Mutable, value_type*, const value_type*>;

  public:
    explicit by_value(value_ptr_t valptr)
      : m_valptr(valptr)
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

    void fill(const value_type& v)
    {
      if (!is_fill_supported())
        throw std::logic_error("Attempting to use fill on an extension that is not fillable!");

      *m_valptr = v;
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
    value_ptr_t m_valptr;
  };

} // namespace mln::extension
