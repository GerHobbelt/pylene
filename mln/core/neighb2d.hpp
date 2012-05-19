#ifndef MLN_CORE_NEIGHB2D_HPP
# define MLN_CORE_NEIGHB2D_HPP

# include <mln/core/ndneighborhood.hpp>
# include <mln/core/point.hpp>

namespace mln {


  struct c4_ : neighborhood_base< c4_, point2d >
  {
    static const int static_size = 4;
    static const point2d dpoints[4];
  };

  const point2d c4_::dpoints[4] = { {{-1,0}}, {{0,-1}}, {{0,1}}, {{1,0}} };

  static c4_ c4;


} // end of namespace mln

#endif //!MLN_CORE_NEIGHB2D_HPP
