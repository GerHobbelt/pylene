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
    T compute_periodized_coord(T pnt, std::size_t shp_)
    {
      if (shp_ == 0)
        throw std::runtime_error("Division by zero!");

      auto shp = static_cast<T>(shp_);

      if (pnt < 0)
        while (pnt < 0)
          pnt += shp;

      return pnt % shp;
    }

  } // namespace detail

  template <typename V, typename I>
  struct by_periodize
  {
    using value_type        = V;
    using point_type        = image_point_t<I>;
    using support_fill      = std::false_type;
    using support_mirror    = std::false_type;
    using support_periodize = std::true_type;
    using support_clamp     = std::false_type;
    using support_buffer    = std::false_type;

    explicit by_periodize(I ima)
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

    constexpr void periodize()
    {
      // Nothing to do, everything is lazy-computed
    }

    const V& value(const point_type& pnt) const
    {
      using domain_t     = typename I::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return m_ima(compute_coords<dim>(std::move(pnt), m_ima.domain().shape()));
    }

  private:
    template <std::size_t dim, typename Pnt>
    Pnt compute_coords(Pnt pnt, Pnt shp) const
    {
      return compute_coords_impl(std::move(pnt), std::move(shp), std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... Idx>
    Pnt compute_coords_impl(Pnt pnt, Pnt shp, std::index_sequence<Idx...>) const
    {
      return Pnt(detail::compute_periodized_coord(pnt[Idx], shp[Idx])...);
    }

  private:
    I m_ima;
  };

} // namespace mln::extension
