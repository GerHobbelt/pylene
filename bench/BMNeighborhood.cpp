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
  auto offsets = wrt_delta_index(ima, c8_t::dpoints);
  mln_iter(p, ima.domain());
  mln_iter(n, c8(p));

  long u = 0;
  int nrows = ima.nrows();
  int ncols = ima.ncols();
  for (int i = 0; i < nrows; ++i)
  {
    for (int j = 0; j < ncols; ++j)
    {
      int p = idx + j; // * ima.index_strides()[1];
      int sum = 0;
      for (int offset : offsets)
        sum += ima[p + offset];
      u += sum;
    }
    idx += ima.index_strides()[0];
  }
  return u;
}


long bench_pointers(const image2d<int>& ima)
{
  constexpr int sz = 8;

  const size_t stride = ima.index_strides()[0];
  int nrows = ima.nrows();
  int ncols = ima.ncols();
  auto offsets = wrt_delta_index(ima, c8_t::dpoints);

  long u = 0;
  const int* lineptr = &ima(ima.domain().pmin);
  for (int y = 0; y < nrows; ++y, lineptr += stride)
    for (int x = 0; x < ncols; ++x)
      {
        int sum = 0;
        for (int k = 0; k < sz; ++k)
          sum += lineptr[x + offsets[k]];
        u += sum;
      }

  return u;
}

