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
    int compute_mirrored_coord(int pnt, std::size_t shp_, std::size_t /*padding*/)
    {
      if (shp_ == 0)
        throw std::runtime_error("Division by zero!");

      auto shp = static_cast<int>(shp_);

      while (pnt < (2 * shp))
        pnt += 2 * shp;

      pnt %= 2 * shp;
      // pnt -= padding;

      if (pnt < shp)
        return pnt;
      else // pnt >= shp
        return 2 * shp - 1 - pnt;
    }

    int compute_periodized_coord(int pnt, std::size_t shp_)
    {
      if (shp_ == 0)
        throw std::runtime_error("Division by zero!");

      auto shp = static_cast<int>(shp_);

      if (pnt < 0)
        while (pnt < 0)
          pnt += shp;

      return pnt % shp;
    }

    int compute_clamped_coord(int pnt, std::size_t shp_)
    {
      if (shp_ == 0)
        throw std::runtime_error("Division by zero!");
      if (pnt < 0)
        return 0;

      auto shp = static_cast<int>(shp_);

      if (pnt < shp - 1)
        return pnt;
      else
        return shp - 1;
    }
  } // namespace detail

  template <typename V, typename I>
  struct by_pattern
  {
    using value_type        = V;
    using point_type        = image_point_t<I>;
    using support_fill      = std::false_type;
    using support_mirror    = std::true_type;
    using support_periodize = std::true_type;
    using support_clamp     = std::true_type;
    using support_buffer    = std::false_type;

    by_pattern(I ima, mln::extension::experimental::Pattern pattern, std::size_t padding = 0)
      : m_ima{std::move(ima)}
      , m_padding{padding}
      , m_pattern{pattern}
    {
    }

    std::size_t padding() const { return m_padding; }

    experimental::Pattern pattern() const { return m_pattern; }

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
      m_pattern = experimental::Pattern::Mirror;
      m_padding = padding;
    }

    constexpr void periodize() { m_pattern = experimental::Pattern::Periodize; }

    constexpr void clamp() { m_pattern = experimental::Pattern::Clamp; }


    const V& value(const point_type& pnt) const
    {
      switch (m_pattern)
      {
      case experimental::Pattern::Mirror:
        return value_mirror(std::move(pnt));
      case experimental::Pattern::Periodize:
        return value_periodize(std::move(pnt));
      case experimental::Pattern::Clamp:
        return value_clamp(std::move(pnt));
      default:
        throw std::runtime_error("Unsupported extension pattern!");
      }
    }

  private:
    const V& value_mirror(image_point_t<I> pnt) const
    {
      using domain_t     = typename I::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return m_ima(compute_mirror_coords<dim>(std::move(pnt), m_ima.domain().shape(), this->m_padding));
    }

    template <std::size_t dim, typename Pnt>
    Pnt compute_mirror_coords(Pnt pnt, Pnt shp, std::size_t padding) const
    {
      return compute_mirror_coords_impl(std::move(pnt), std::move(shp), std::move(padding),
                                        std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... Idx>
    Pnt compute_mirror_coords_impl(Pnt pnt, Pnt shp, std::size_t padding, std::index_sequence<Idx...>) const
    {
      if ((((shp[Idx] - padding) <= 0) || ...))
        throw std::runtime_error(
            "Cannot have a padding whose size is supperior or equal to one of the dimension's shape.");

      // auto p = Pnt(detail::compute_mirrored_coord(pnt[Idx], shp[Idx], padding)...);
      // std::cout << pnt << " -> " << p << std::endl;
      // return p;

      return Pnt(detail::compute_mirrored_coord(pnt[Idx], shp[Idx], padding)...);
    }

    const V& value_periodize(image_point_t<I> pnt) const
    {
      using domain_t     = typename I::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return m_ima(compute_periodize_coords<dim>(std::move(pnt), m_ima.domain().shape()));
    }

    template <std::size_t dim, typename Pnt>
    Pnt compute_periodize_coords(Pnt pnt, Pnt shp) const
    {
      return compute_periodize_coords_impl(std::move(pnt), std::move(shp), std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... Idx>
    Pnt compute_periodize_coords_impl(Pnt pnt, Pnt shp, std::index_sequence<Idx...>) const
    {
      return Pnt(detail::compute_periodized_coord(pnt[Idx], shp[Idx])...);
    }

    const V& value_clamp(image_point_t<I> pnt) const
    {
      using domain_t     = typename I::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return m_ima(compute_clamp_coords<dim>(std::move(pnt), m_ima.domain().shape()));
    }

    template <std::size_t dim, typename Pnt>
    Pnt compute_clamp_coords(Pnt pnt, Pnt shp) const
    {
      return compute_clamp_coords_impl(std::move(pnt), std::move(shp), std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... Idx>
    Pnt compute_clamp_coords_impl(Pnt pnt, Pnt shp, std::index_sequence<Idx...>) const
    {
      return Pnt(detail::compute_clamped_coord(pnt[Idx], shp[Idx])...);
    }

  private:
    I                                     m_ima;
    std::size_t                           m_padding;
    mln::extension::experimental::Pattern m_pattern;
  };

} // namespace mln::extension
