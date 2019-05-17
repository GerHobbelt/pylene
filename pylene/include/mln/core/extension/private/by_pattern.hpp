#pragma once

#include <mln/core/concept/new/domains.hpp>
#include <mln/core/concept/new/structuring_elements.hpp>

#include <algorithm>
#include <type_traits>
#include <utility>


namespace mln::extension
{

  template <typename V>
  struct by_pattern
  {
    enum class Pattern
    {
      Mirror,
      Periodize,
      Clamp
    };

    using value_type        = V;
    using support_fill      = std::false_type;
    using support_mirror    = std::true_type;
    using support_periodize = std::true_type;
    using support_clamp     = std::true_type;
    using is_finite         = std::false_type;

    explicit by_pattern(Pattern p, std::size_t padding = 0)
      : m_pattern(p)
      , m_padding(padding)
    {
    }

    template <typename SE>
    bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      return true;
    }

    void mirror(std::size_t padding)
    {
      m_pattern = Pattern::Mirror;
      m_padding = padding;
    }

    void periodize() { m_pattern = Pattern::Periodize; }

    void clamp() { return m_pattern = Pattern::Clamp; }

    Pattern pattern() const { return m_pattern; }

    std::size_t padding() const { return m_padding; }

    template <typename Pnt, typename Ima>
    const V& value(Pnt pnt, const Ima& ima) const
    {
      switch (m_pattern)
      {
      case Pattern::Mirror:
        return value_mirror(std::move(pnt), ima);
      case Pattern::Periodize:
        return value_periodize(std::move(pnt), ima);
      case Pattern::Clamp:
        return value_clamp(std::move(pnt), ima);
      }
    }

  private:
    template <typename Pnt, typename Ima>
    const V& value_mirror(Pnt pnt, const Ima& ima) const
    {
      using domain_t     = typename Ima::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return ima(compute_mirror_coords<dim>(std::move(pnt), ima.domain().shape(), m_padding));
    }

    template <std::size_t dim, typename Pnt>
    Pnt compute_mirror_coords(Pnt pnt, Pnt shp, std::size_t padding) const
    {
      return compute_mirror_coords_impl(std::move(pnt), std::move(shp), std::move(padding),
                                        std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... I>
    Pnt compute_mirror_coords_impl(Pnt pnt, Pnt shp, std::size_t padding, std::index_sequence<I...>) const
    {
      return {(shp[I] - pnt[I] % (shp[I] - padding))...};
    }

    template <typename Pnt, typename Ima>
    const V& value_periodize(Pnt pnt, const Ima& ima) const
    {
      using domain_t     = typename Ima::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return ima(compute_periodize_coords<dim>(std::move(pnt), ima.domain().shape()));
    }

    template <std::size_t dim, typename Pnt>
    Pnt compute_periodize_coords(Pnt pnt, Pnt shp) const
    {
      return compute_periodize_coords_impl(std::move(pnt), std::move(shp), std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... I>
    Pnt compute_periodize_coords_impl(Pnt pnt, Pnt shp, std::index_sequence<I...>) const
    {
      return {(pnt[I] % shp[I])...};
    }

    template <typename Pnt, typename Ima>
    const V& value_clamp(Pnt pnt, const Ima& ima) const
    {
      using domain_t     = typename Ima::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return ima(compute_clamp_coords<dim>(std::move(pnt), ima.domain().shape()));
    }

    template <std::size_t dim, typename Pnt>
    Pnt compute_clamp_coords(Pnt pnt, Pnt shp) const
    {
      return compute_clamp_coords_impl(std::move(pnt), std::move(shp), std::make_index_sequence<dim>{});
    }

    template <typename Pnt, std::size_t... I>
    Pnt compute_clamp_coords_impl(Pnt pnt, Pnt shp, std::index_sequence<I...>) const
    {
      using std::min;
      return {min(pnt[I], shp[I])...};
    }

    Pattern     m_pattern;
    std::size_t m_padding;
  };

} // namespace mln::extension
