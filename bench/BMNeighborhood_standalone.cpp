#include <mln/core/image/image2d.hpp>
#include <mln/core/win2d.hpp>
#include <mln/core/wrt_offset.hpp>

using namespace mln;

void
erode_c8_1(const mln::image2d<mln::uint8>& f, mln::image2d<mln::uint8>& out)
{
  mln_pixter(px, f);
  mln_pixter(pxout, out);
  mln_iter(nx, winc8(px));

  mln_forall(px,pxout)
  {
    uint8 min = 255;
    mln_forall(nx)
      min = std::min(min, nx->val());
    pxout->val() = min;
  }
}

void
erode_c8_2(const mln::image2d<mln::uint8>& f, mln::image2d<mln::uint8>& out)
{
  constexpr int sz = 9;
  auto dpoints = winc8_t::dpoints;

  const uint8* __restrict__ inptr = &f({0, 0});
  uint8* __restrict__ outptr = &out({0, 0});

  ptrdiff_t offsets[sz];
  wrt_offset(f, dpoints, offsets);

  int stride = f.strides()[0];
  int nr = f.nrows();
  int nc = f.ncols();

  for (int x = 0; x < nr; ++x)
  {
#pragma clang loop vectorize(enable)
    for (int y = 0; y < nc; ++y)
    {
      int idx = x * stride + y;
      uint8 min = 255;
      for (int k = 0; k < sz; ++k)
        min = std::min(min, inptr[idx + offsets[k]]);
      outptr[idx] = min;
    }
  }
}
