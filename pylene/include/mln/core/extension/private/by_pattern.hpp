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
  namespace detail
  {
    template <mln::extension::experimental::Pattern P, typename = void>
    class mirror_pattern_data
    {
    };

    template <mln::extension::experimental::Pattern P>
    class mirror_pattern_data<P, std::enable_if_t<(P == experimental::Pattern::Mirror)>>
    {
    public:
      explicit mirror_pattern_data(std::size_t padding)
        : m_padding(padding)
      {
      }

      std::size_t padding() const { return m_padding; }

    protected:
      std::size_t m_padding;
    };
  } // namespace detail

  template <typename V, mln::extension::experimental::Pattern P>
  struct by_pattern : public detail::mirror_pattern_data<P>
  {
    using value_type                                                 = V;
    static constexpr mln::extension::experimental::Pattern m_pattern = P;
    using support_fill                                               = std::false_type;
    using support_mirror    = std::bool_constant<(P == experimental::Pattern::Mirror)>;
    using support_periodize = std::bool_constant<(P == experimental::Pattern::Periodize)>;
    using support_clamp     = std::bool_constant<(P == experimental::Pattern::Clamp)>;
    using is_finite         = std::false_type;

    template <typename U = void, typename = std::enable_if_t<(P == experimental::Pattern::Mirror)>>
    explicit by_pattern(std::size_t padding = 0)
      : detail::mirror_pattern_data<P>{padding}
    {
    }

    template <typename U = void, typename = std::enable_if_t<(P != experimental::Pattern::Mirror)>>
    by_pattern()
    {
    }

    template <typename SE>
    bool fit(const SE&) const
    {
      PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

      return true;
    }

    template <typename U = void, typename = std::enable_if_t<(P == experimental::Pattern::Mirror)>>
    void mirror(std::size_t padding = 0)
    {
      this->m_padding = padding;
      // Nothing to do, lazy evaluation when value is yielded
    }

    template <typename U = void, typename = std::enable_if_t<(P == experimental::Pattern::Periodize)>>
    void periodize()
    {
      // Nothing to do, lazy evaluation when value is yielded
    }

    template <typename U = void, typename = std::enable_if_t<(P == experimental::Pattern::Clamp)>>
    void clamp()
    {
      // Nothing to do, lazy evaluation when value is yielded
    }

    static constexpr experimental::Pattern pattern() { return m_pattern; }

    template <typename Ima>
    const V& value(image_point_t<Ima> pnt, const Ima& ima) const
    {
      if constexpr (m_pattern == experimental::Pattern::Mirror)
      {
        return value_mirror(std::move(pnt), ima);
      }
      else if constexpr (m_pattern == experimental::Pattern::Periodize)
      {
        return value_periodize(std::move(pnt), ima);
      }
      else if constexpr (m_pattern == experimental::Pattern::Clamp)
      {
        return value_clamp(std::move(pnt), ima);
      }
    }

  private:
    template <typename Ima>
    const V& value_mirror(image_point_t<Ima> pnt, const Ima& ima) const
    {
      using domain_t     = typename Ima::domain_type;
      constexpr auto dim = domain_t::dimension;

      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_t>,
                               "Domain must be shaped to allow pattern-based extension!");

      return ima(compute_mirror_coords<dim>(std::move(pnt), ima.domain().shape(), this->m_padding));
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

    template <typename Ima>
    const V& value_periodize(image_point_t<Ima> pnt, const Ima& ima) const
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

    template <typename Ima>
    const V& value_clamp(image_point_t<Ima> pnt, const Ima& ima) const
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
  };

} // namespace mln::extension
