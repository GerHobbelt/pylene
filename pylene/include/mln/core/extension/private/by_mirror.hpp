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
    int compute_mirrored_coord(int pnt, std::size_t shp_, std::size_t padding)
    {
      if (shp_ == 0)
        throw std::runtime_error("Division by zero!");

      auto shp = static_cast<int>(shp_);

      if (pnt < 0)
        pnt -= padding;
      else if (pnt >= shp)
        pnt += padding;

      while (pnt < (2 * shp))
        pnt += 2 * shp;

      pnt %= 2 * shp;

      if (pnt < shp)
        return pnt;
      else // pnt >= shp
        return 2 * shp - 1 - pnt;
    }
  } // namespace detail

  template <typename V, typename I>
  struct by_mirror
  {
    using value_type        = V;
    using point_type        = image_point_t<I>;
    using support_fill      = std::false_type;
    using support_mirror    = std::true_type;
    using support_periodize = std::false_type;
    using support_clamp     = std::false_type;
    using support_buffer    = std::false_type;

    explicit by_mirror(I ima, std::size_t padding = 0)
      : m_ima{std::move(ima)}
      , m_padding{padding}
    {
    }

    std::size_t padding() const { return m_padding; }

    template <typename SE>
    constexpr bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      return true;
    }

    constexpr bool is_finite() const { return false; }

    constexpr std::optional<std::size_t> size() const { return std::nullopt; }

    constexpr void mirror(std::size_t padding = 0)
    {
      m_padding = padding;
      // Nothing to do, everything is lazy-computed
    }


    const V& value(const point_type& pnt) const
    {
      using domain_t     = typename I::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return m_ima(compute_coords<dim>(std::move(pnt), m_ima.domain().shape(), this->m_padding));
    }

  private:
    template <std::size_t dim, typename Pnt>
    Pnt compute_coords(Pnt pnt, Pnt shp, std::size_t padding) const
    {
      return compute_coords_impl(std::move(pnt), std::move(shp), std::move(padding), std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... Idx>
    Pnt compute_coords_impl(Pnt pnt, Pnt shp, std::size_t padding, std::index_sequence<Idx...>) const
    {
      if ((((shp[Idx] - padding) <= 0) || ...))
        throw std::runtime_error(
            "Cannot have a padding whose size is supperior or equal to one of the dimension's shape.");

      return Pnt(detail::compute_mirrored_coord(pnt[Idx], shp[Idx], padding)...);
    }

  private:
    I           m_ima;
    std::size_t m_padding;
  };

} // namespace mln::extension