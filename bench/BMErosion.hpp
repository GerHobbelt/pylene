#include <mln/core/image/image2d.hpp>

void erode_c8_mlnstyle(const mln::image2d<mln::uint8>& f, mln::image2d<mln::uint8>& out);

void erode_c8_kernel(const mln::image2d<mln::uint8>& f, mln::image2d<mln::uint8>& out);

void erode_c8_cstyle(const mln::image2d<mln::uint8>& f, mln::image2d<mln::uint8>& out);

void erode_c8_cstyle_restrict(const mln::image2d<mln::uint8>& f, mln::image2d<mln::uint8>& out);
