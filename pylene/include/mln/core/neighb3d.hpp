#ifndef MLN_CORE_NEIGHB3D_HPP
#define MLN_CORE_NEIGHB3D_HPP

#include <array>
#include <mln/core/neighborhood/dyn_neighborhood.hpp>
#include <mln/core/point.hpp>


namespace mln
{

  namespace
  {

    struct c6_t : dyn_neighborhood_base<constant_neighborhood_tag, c6_t>
    {
      static const int                    static_size = 6;
      static const std::array<point3d, 6> dpoints;

      const auto& offsets() const { return dpoints; }
    };
    const std::array<point3d, 6> c6_t::dpoints = {
        {{-1, 0, 0}, {0, -1, 0}, {0, 0, -1}, {0, 0, 1}, {0, 1, 0}, {1, 0, 0}}};

    struct c26_t : dyn_neighborhood_base<constant_neighborhood_tag, c26_t>
    {
      static const int                     static_size = 26;
      static const std::array<point3d, 26> dpoints;

      const auto& offsets() const { return dpoints; }
    };

    const std::array<point3d, 26> c26_t::dpoints = {{{-1, -1, -1},
                                                     {-1, -1, 0},
                                                     {-1, -1, 1},
                                                     {-1, 0, -1},
                                                     {-1, 0, 0},
                                                     {-1, 0, 1},
                                                     {-1, 0, -1},
                                                     {-1, 0, 0},
                                                     {-1, 0, 1},
                                                     {0, -1, -1},
                                                     {0, -1, 0},
                                                     {0, -1, 1},
                                                     {0, 0, -1},
                                                     /*{ 0,  0,  0},*/
                                                     {0, 0, 1},
                                                     {0, 1, -1},
                                                     {0, 1, 0},
                                                     {0, 1, 1},
                                                     {1, -1, -1},
                                                     {1, -1, 0},
                                                     {1, -1, 1},
                                                     {1, 0, -1},
                                                     {1, 0, 0},
                                                     {1, 0, 1},
                                                     {1, 1, -1},
                                                     {1, 1, 0},
                                                     {1, 1, 1}}};
  }


  static const c6_t  c6{};
  static const c26_t c26{};

} // namespace mln

#endif //! MLN_CORE_NEIGHB3D_HPP
