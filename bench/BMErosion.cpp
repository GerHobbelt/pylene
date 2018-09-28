#ifdef _MSC_VER
#define __restrict__ __restrict
#endif

#include <mln/core/image/image2d.hpp>
#include <mln/core/win2d.hpp>
#include <mln/core/wrt_offset.hpp>
#include <mln/morpho/structural/erode.hpp>

using namespace mln;

void erode_c8_mlnstyle(const mln::image2d<mln::uint8>& f, mln::image2d<mln::uint8>& out)
{
  mln_pixter(px, f);
  mln_pixter(pxout, out);
  mln_iter(nx, winc8(px));

  mln_forall (px, pxout)
  {
    uint8 min = 255;
    //#pragma clang loop vectorize(enable)
    mln_simple_forall(nx) min = std::min(min, nx->val());
    pxout->val() = min;
  }
}

void erode_c8_kernel(const mln::image2d<mln::uint8>& f, mln::image2d<mln::uint8>& out)
{
  (void)f;
  (void)out;
  mln::morpho::structural::erode(f, winc8, out, std::less<mln::uint8>());
}

void erode_c8_cstyle(const mln::image2d<mln::uint8>& f, mln::image2d<mln::uint8>& out)
{
  constexpr int sz = 9;
  auto dpoints = winc8_t::dpoints;

  const uint8* __restrict__ inptr = &f({0, 0});
  uint8* __restrict__ outptr = &out({0, 0});

  ptrdiff_t offsets[sz];
  wrt_offset(f, dpoints, offsets);

  int stride = static_cast<int>(f.strides()[0]);
  int nr = f.nrows();
  int nc = f.ncols();

  for (int y = 0; y < nr; ++y)
  {
    const uint8* inlineptr = inptr + stride * y;
    uint8* outlineptr = outptr + stride * y;
    for (int x = 0; x < nc; ++x)
    {
      uint8 min = 255;
      for (int k = 0; k < sz; ++k)
        min = std::min(min, inlineptr[x + offsets[k]]);
      outlineptr[x] = min;
    }
  }
}

void erode_c8_cstyle_restrict(const mln::image2d<mln::uint8>& f, mln::image2d<mln::uint8>& out)
{
  constexpr int sz = 9;
  auto dpoints = winc8_t::dpoints;

  const uint8* __restrict__ inptr = &f({0, 0});
  uint8* __restrict__ outptr = &out({0, 0});

  ptrdiff_t offsets[sz];
  wrt_offset(f, dpoints, offsets);

  int stride = static_cast<int>(f.strides()[0]);
  int nr = f.nrows();
  int nc = f.ncols();

  auto applyLine = [nc, offsets](const uint8* __restrict__ inlineptr, uint8* __restrict__ outlineptr) {
    for (int x = 0; x < nc; ++x)
    {
      uint8 min = 255;
      for (int k = 0; k < sz; ++k)
        min = std::min(min, inlineptr[x + offsets[k]]);
      outlineptr[x] = min;
    }
  };

  for (int y = 0; y < nr; ++y)
  {
    const uint8* inlineptr = inptr + stride * y;
    uint8* outlineptr = outptr + stride * y;
    applyLine(inlineptr, outlineptr);
  }
}
