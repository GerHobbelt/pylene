#ifndef MLN_CORE_WIN2D_HPP
#define MLN_CORE_WIN2D_HPP

#include <mln/core/assert.hpp>
#include <mln/core/domain/box.hpp>
#include <mln/core/neighborhood/dyn_neighborhood.hpp>
#include <mln/core/point.hpp>

///
/// \file
///

namespace mln
{

  namespace
  {

    struct winc4_t : dyn_neighborhood_base<constant_neighborhood_tag, winc4_t>
    {
      constexpr const std::array<point2d, 5>& offsets() const { return dpoints; }
      static const int static_size = 5;
      static const std::array<point2d, 5> dpoints;
    };

    const std::array<point2d, 5> winc4_t::dpoints = {{{-1, 0}, {0, -1}, {0, 0}, {0, 1}, {1, 0}}};

    static const winc4_t winc4{};

    struct winc8_t : dyn_neighborhood_base<constant_neighborhood_tag, winc8_t>
    {
      constexpr const std::array<point2d, 9>& offsets() const { return dpoints; }
      static const int static_size = 9;
      static const std::array<point2d, 9> dpoints;
    };

    const std::array<point2d, 9> winc8_t::dpoints = {
        {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 1}, {1, 0}}};

    static const winc8_t winc8{};

    struct winc2_v_t : dyn_neighborhood_base<constant_neighborhood_tag, winc2_v_t>
    {
      constexpr const std::array<point2d, 3>& offsets() const { return dpoints; }
      static const int static_size = 3;
      static const std::array<point2d, 3> dpoints;
    };

    const std::array<point2d, 3> winc2_v_t::dpoints = {{{-1, 0}, {0, 0}, {1, 0}}};

    static const winc2_v_t winc2_v{};

    struct winc2_h_t : dyn_neighborhood_base<constant_neighborhood_tag, winc2_h_t>
    {
      constexpr const std::array<point2d, 3>& offsets() const { return dpoints; }
      static const int static_size = 3;
      static const std::array<point2d, 3> dpoints;
    };

    const std::array<point2d, 3> winc2_h_t::dpoints = {{{0, -1}, {0, 0}, {0, 1}}};

    static const winc2_h_t winc2_h{};
  }

}

#endif // !  MLN_CORE_WIN2D_HPP
