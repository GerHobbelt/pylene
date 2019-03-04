#pragma once

#include <mln/core/neighborhood/dyn_neighborhood.hpp>
#include <mln/core/point.hpp>

#include <array>


namespace mln
{

  namespace
  {

    struct c4_t : dyn_neighborhood_base<constant_neighborhood_tag, c4_t>
    {
      constexpr const std::array<point2d, 4>& offsets() const { return dpoints; }

      static const int                    static_size = 4;
      static const std::array<point2d, 4> dpoints;
    };
    const std::array<point2d, 4> c4_t::dpoints = {{{-1, 0}, {0, -1}, {0, 1}, {1, 0}}};

    struct c8_t : dyn_neighborhood_base<constant_neighborhood_tag, c8_t>
    {
      constexpr const std::array<point2d, 8>& offsets() const { return dpoints; }

      static const int                    static_size = 8;
      static const std::array<point2d, 8> dpoints;
    };

    const std::array<point2d, 8> c8_t::dpoints = {
        {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

    struct c2_v_t : dyn_neighborhood_base<constant_neighborhood_tag, c2_v_t>
    {
      constexpr const std::array<point2d, 2>& offsets() const { return dpoints; }

      static const int                    static_size = 2;
      static const std::array<point2d, 2> dpoints;
    };
    const std::array<point2d, 2> c2_v_t::dpoints = {{{-1, 0}, {1, 0}}};

    struct c2_h_t : dyn_neighborhood_base<constant_neighborhood_tag, c2_h_t>
    {
      constexpr const std::array<point2d, 2>& offsets() const { return dpoints; }

      static const int                    static_size = 2;
      static const std::array<point2d, 2> dpoints;
    };
    const std::array<point2d, 2> c2_h_t::dpoints = {{{0, -1}, {0, 1}}};
  } // namespace

  static const c4_t   c4{};
  static const c8_t   c8{};
  static const c2_v_t c2_v{};
  static const c2_h_t c2_h{};

} // namespace mln
