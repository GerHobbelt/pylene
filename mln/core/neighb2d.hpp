#ifndef MLN_CORE_NEIGHB2D_HPP
# define MLN_CORE_NEIGHB2D_HPP

# include <mln/core/neighborhood.hpp>
# include <mln/core/std/array.hpp>
# include <mln/core/point.hpp>


namespace mln {


  struct c4_t : neighborhood_base< c4_t, mln::array<point2d, 4> >
  {
    static const int static_size = 4;
    static constexpr mln::array<point2d, 4> dpoints = {{{ {-1,0}, {0,-1}, {0,1}, {1,0} }}};
  };

  //mln::array<point2d, 4> c4_t::dpoints;
  constexpr mln::array<point2d, 4> c4_t::dpoints;

  static const c4_t c4;

  struct c8_t : neighborhood_base< c8_t, mln::array<point2d, 8> >
  {
    static const int static_size = 8;
    static constexpr mln::array<point2d, 8> dpoints = {{{ {-1,-1}, {-1,0}, {-1,1},
                                                          {0, -1},         {0, 1},
                                                          {1,-1},  {1,1},  {1,0} }}};
  };

  //constexpr mln::array<point2d, 8> c8_t::dpoints = 
  constexpr mln::array<point2d, 8> c8_t::dpoints;

  static const c8_t c8;


} // end of namespace mln

#endif //!MLN_CORE_NEIGHB2D_HPP
