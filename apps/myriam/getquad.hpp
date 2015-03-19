#ifndef APPS_SMARTDOC_GETQUAD_HPP
# define APPS_SMARTDOC_GETQUAD_HPP

# include <mln/core/image/image2d.hpp>

struct pquad_t
{
  mln::point2df tl, tr, bl, br;
};



/// Return the best fitting quad in a binary image
/// \param[in] f The input image
/// \param[in] debug_filename (optional) Output the detected quad as an image.
/// \return The points in the order (top left, top right, bottom left, bottom right)
pquad_t
getquad(const mln::image2d<bool>& f,
        const char* debug_filename = nullptr);





#endif // !APPS_SMARTDOC_GETQUAD_HPP
