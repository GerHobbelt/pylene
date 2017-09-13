#include <benchmark/benchmark.h>

#include <mln/core/image/image2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/wrt_offset.hpp>

//#include <mln/core/neighborhood/sliding_viter.hpp>
using namespace mln;

long bench_pixter(const image2d<int>& ima)
{
  mln_pixter(px, ima);
  mln_iter(nx, c8(px));

  long u = 0;
  mln_forall(px)
    mln_forall(nx)
    u += nx->val();

  return u;
}

long bench_piter(const image2d<int>& ima)
{
  mln_iter(p, ima.domain());
  mln_iter(n, c8(p));

  long u = 0;
  mln_forall(p)
    mln_forall(n)
      u += ima.at(*n);

  return u;
}

long bench_indexes(const image2d<int>& ima)
{
  std::size_t idx = ima.index_of_point(ima.domain().pmin);
  auto w = wrt_delta_index(ima, c8_t::dpoints);
  mln_iter(p, ima.domain());
  mln_iter(n, c8(p));

  long u = 0;
  unsigned nrows = ima.nrows();
  unsigned ncols = ima.ncols();
  for (unsigned i = 0; i < nrows; ++i)
  {
    for (unsigned j = 0; j < ncols; ++j)
    {
      std::size_t p = idx + j; // * ima.index_strides()[1];
      mln_foreach (auto k, w)
        u += ima[p + k];
    }
    idx += ima.index_strides()[0];
  }
  return u;
}


long bench_pointers(const image2d<int>& ima)
{
  constexpr int sz = 8;
  auto dpoints = c8_t::dpoints;

  const char* ptr2 = (char*)&ima(ima.domain().pmin);
  const size_t strides[2] = {ima.strides()[0], ima.strides()[1]};
  const point2d pmax = ima.domain().shape();
  ptrdiff_t offsets[8];
  wrt_offset(ima, dpoints, offsets);

  long u = 0;
  for (int x = 0; x < pmax[0]; ++x, ptr2 += strides[0])
  {
    const char* ptr = ptr2;
    for (int y = 0; y < pmax[1]; ++y, ptr += strides[1])
      for (int k = 0; k < sz; ++k)
        u += *(const int*)(ptr + offsets[k]);
  }

  return u;
}

