#include <mln/core/image/image2d.hpp>


unsigned threshold1(const mln::image2d<mln::uint8>& f, mln::uint8 v);
unsigned threshold1_bis(const mln::image2d<mln::uint8>& f, mln::uint8 v);
unsigned threshold2(const mln::image2d<mln::uint8>& f, mln::uint8 v);
unsigned threshold3(const mln::image2d<mln::uint8>& f, mln::uint8 v);
unsigned threshold4(const mln::image2d<mln::uint8>& f, mln::uint8 v);
void     threshold5(const mln::image2d<mln::uint8>& f,
                      mln::image2d<bool>& out,
                      mln::uint8 v);
void     threshold6(const mln::image2d<mln::uint8>& f,
                      mln::image2d<bool>& out,
                      mln::uint8 v);
