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

  constexpr mln::array<point2d, 4> c4_t::dpoints;

  static const c4_t c4;

  struct c8_t : neighborhood_base< c8_t, mln::array<point2d, 8> >
  {
    static const int static_size = 8;
    static constexpr mln::array<point2d, 8> dpoints = {{{ {-1,-1}, {-1,0}, {-1,1},
                                                          {0, -1},         {0, 1},
                                                          {1,-1},  {1,1},  {1,0} }}};
  };

  constexpr mln::array<point2d, 8> c8_t::dpoints;

  static const c8_t c8;

  struct c2_v_t : neighborhood_base< c2_v_t, mln::array<point2d, 2> >
  {
    static const int static_size = 2;
    static constexpr mln::array<point2d, 2> dpoints = {{{ {-1,0}, {1,0} }}};
  };

  constexpr mln::array<point2d, 2> c2_v_t::dpoints;

  static const c2_v_t c2_v;



  struct c2_h_t : neighborhood_base< c2_h_t, mln::array<point2d, 2> >
  {
    static const int static_size = 2;
    static constexpr mln::array<point2d, 2> dpoints = {{{ {0,-1}, {0,1} }}};
  };

  constexpr mln::array<point2d, 2> c2_h_t::dpoints;

  static const c2_h_t c2_h;


} // end of namespace mln

#endif //!MLN_CORE_NEIGHB2D_HPP
