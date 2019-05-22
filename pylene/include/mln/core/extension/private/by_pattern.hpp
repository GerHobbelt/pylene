#pragma once

#include <mln/core/concept/new/domains.hpp>
#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/image/image.hpp>

#include <algorithm>
#include <type_traits>
#include <utility>


namespace mln::extension
{
  template <typename V, typename I>
  struct by_pattern
  {
    using value_type        = V;
    using point_type        = image_point_t<I>;
    using support_fill      = std::false_type;
    using support_mirror    = std::true_type;
    using support_periodize = std::true_type;
    using support_clamp     = std::true_type;
    using is_finite         = std::false_type;

    by_pattern(I* ima, mln::extension::experimental::Pattern pattern, std::size_t padding = 0)
      : m_ima{ima}
      , m_pattern{pattern}
      , m_padding{padding}
    {
    }

    std::size_t           padding() const { return m_padding; }
    experimental::Pattern pattern() const { return m_pattern; }

    template <typename SE>
    bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      return true;
    }

    void mirror(std::size_t padding = 0)
    {
      m_pattern = experimental::Pattern::Mirror;
      m_padding = padding;
    }
    void periodize() { m_pattern = experimental::Pattern::Periodize; }
    void clamp() { m_pattern = experimental::Pattern::Clamp; }


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

      return (*m_ima)(compute_mirror_coords<dim>(std::move(pnt), m_ima->domain().shape(), this->m_padding));
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
      return Pnt((shp[Idx] - pnt[Idx] % (shp[Idx] - padding))...);
    }

    const V& value_periodize(image_point_t<I> pnt) const
    {
      using domain_t     = typename I::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return (*m_ima)(compute_periodize_coords<dim>(std::move(pnt), m_ima->domain().shape()));
    }

    template <std::size_t dim, typename Pnt>
    Pnt compute_periodize_coords(Pnt pnt, Pnt shp) const
    {
      return compute_periodize_coords_impl(std::move(pnt), std::move(shp), std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... Idx>
    Pnt compute_periodize_coords_impl(Pnt pnt, Pnt shp, std::index_sequence<Idx...>) const
    {
      return Pnt((pnt[Idx] % shp[Idx])...);
    }

    const V& value_clamp(image_point_t<I> pnt) const
    {
      using domain_t     = typename I::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return (*m_ima)(compute_clamp_coords<dim>(std::move(pnt), m_ima->domain().shape()));
    }

    template <std::size_t dim, typename Pnt>
    Pnt compute_clamp_coords(Pnt pnt, Pnt shp) const
    {
      return compute_clamp_coords_impl(std::move(pnt), std::move(shp), std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... Idx>
    Pnt compute_clamp_coords_impl(Pnt pnt, Pnt shp, std::index_sequence<Idx...>) const
    {
      using std::min;
      return Pnt(min(pnt[Idx], shp[Idx])...);
    }

  private:
    I*                                    m_ima;
    std::size_t                           m_padding;
    mln::extension::experimental::Pattern m_pattern;
  };

} // namespace mln::extension
