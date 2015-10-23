#ifndef MLN_CORE_SE_UTILITY_HPP
# define MLN_CORE_SE_UTILITY_HPP

# include <mln/core/neighborhood/dyn_neighborhood.hpp>

// Fwd declaration

namespace mln
{

  namespace se
  {

    template <class SiteSet>
    dyn_neighborhood<SiteSet, dynamic_neighborhood_tag>
    make_se(const SiteSet& s);

    template <class Point, unsigned N>
    dyn_neighborhood<std::array<Point, N>, constant_neighborhood_tag>
    make_se(const std::array<Point, N>& s);

    /******************************************/
    /****          Implementation          ****/
    /******************************************/


    template <class SiteSet>
    dyn_neighborhood<SiteSet, dynamic_neighborhood_tag>
    make_se(const SiteSet& s)
    {
      return {s};
    }

    template <class Point, unsigned N>
    dyn_neighborhood<std::array<Point, N>, constant_neighborhood_tag>
    make_se(const std::array<Point, N>& s)
    {
      return {s};
    }

  } // end of namespace mln::se

} // end of namespace mln

#endif // ! MLN_CORE_SE_UTILITY_HPP
