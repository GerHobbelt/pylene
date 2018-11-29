#ifndef GET_BORDER_FROM_NBH_CPP
#define GET_BORDER_FROM_NBH_CPP

#include <mln/core/foreach.hpp>
#include <mln/core/point.hpp>

namespace mln
{

  namespace internal
  {

    /// \brief Compute the minimal size \p b of the window necessary
    /// to hold the neighbor \p nbh. It returns -1 in this border
    /// cannot be computed.
    ///
    /// Compute the minimal size of the window that contains the neighborhood i.e it
    /// computes b s.t.
    /// \f[
    /// b = \min_\alpha nbh \subset make_win(\alpha)
    /// \f]
    ///
    /// \pre The neighborhood must be non-adaptative.
    /// \pre \p make_win must be increasing
    template <typename Neighborhood, typename SiteSetGenerator>
    int get_border_from_nbh(const Neighborhood& nbh, const SiteSetGenerator& make_win);

    template <typename Neighborhood>
    int get_border_from_nbh(const Neighborhood& nbh);

    /*********************/
    /** Implementation  **/
    /*********************/

    template <typename Neighborhood, typename SiteSetGenerator>
    int get_border_from_nbh(const Neighborhood& nbh, const SiteSetGenerator& make_win)
    {
      unsigned b    = 1;
      auto     cwin = make_win(1);
      mln_foreach (auto dp, nbh.dpoints)
        while (not cwin.has(dp))
          cwin = make_win(++b);
      return b;
    }

    namespace dispatch
    {
      template <typename Neighborhood, typename Site>
      int get_border_from_nbh(const Neighborhood&, const Site&)
      {
        return -1;
      }

      template <typename Neighborhood, typename T, unsigned dim>
      int get_border_from_nbh(const Neighborhood& nbh, const point<T, dim>&)
      {
        unsigned b = 0;
        mln_foreach (auto dp, nbh.offsets())
          for (unsigned i = 0; i < dim; ++i)
            b = std::max<unsigned>(b, std::abs(dp[i]));
        return b;
      }
    }

    template <typename Neighborhood>
    int get_border_from_nbh(const Neighborhood& nbh)
    {
      using P = typename std::remove_reference_t<decltype(nbh.offsets())>::value_type;
      return dispatch::get_border_from_nbh(nbh, P());
    }
  }
}

#endif // ! GET_BORDER_FROM_NBH_CPP
