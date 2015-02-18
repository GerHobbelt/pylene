#ifndef APPS_SMARTDOC_UNHOMO_HPP
# define APPS_SMARTDOC_UNHOMO_HPP

# include <mln/core/image/image2d.hpp>
# include <mln/core/colors.hpp>
# include <array>

// In the order: Top Left, Top Right, Bottom Left, Bottom Right
typedef std::array<mln::point2df, 4> pquad_t;


mln::image2d<mln::rgb8>
unhomo(const mln::image2d<mln::rgb8>& f,
       pquad_t points);


#endif // ! APPS_SMARTDOC_UNHOMO_HPP
