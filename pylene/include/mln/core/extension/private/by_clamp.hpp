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
  namespace detail
  {
    template <typename T>
    T compute_clamped_coord(T pnt, std::size_t shp_)
    {
      if (shp_ == 0)
        throw std::runtime_error("Division by zero!");

      if (pnt < 0)
        return 0;

      auto shp = static_cast<T>(shp_);

      return std::min(pnt, static_cast<T>(shp - 1));
    }
  } // namespace detail

  template <typename V, typename I>
  struct by_clamp
  {
    using value_type        = V;
    using point_type        = image_point_t<I>;
    using support_fill      = std::false_type;
    using support_mirror    = std::false_type;
    using support_periodize = std::false_type;
    using support_clamp     = std::true_type;
    using support_buffer    = std::false_type;

    explicit by_clamp(I ima)
      : m_ima{std::move(ima)}
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

    const V& value(const point_type& pnt) const
    {
      using domain_t     = typename I::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return m_ima(compute_coords<dim>(std::move(pnt), m_ima.domain().shape()));
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

      // Nothing to do, everything is lazy-computed
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
    template <std::size_t dim, typename Pnt>
    Pnt compute_coords(Pnt pnt, Pnt shp) const
    {
      return compute_coords_impl(std::move(pnt), std::move(shp), std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... Idx>
    Pnt compute_coords_impl(Pnt pnt, Pnt shp, std::index_sequence<Idx...>) const
    {
      return Pnt(detail::compute_clamped_coord(pnt[Idx], shp[Idx])...);
    }

  private:
    I m_ima;
  };

} // namespace mln::extension
