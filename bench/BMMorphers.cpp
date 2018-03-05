#include <mln/accu/accumulators/sum.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/utils/ptroffset.hpp>

using namespace mln;

// Threshold A la C
unsigned threshold1(const image2d<uint8>& f, uint8 v)
{
  int nr = f.nrows();
  int nc = f.ncols();
  auto lineptr = f.buffer();
  unsigned count = 0;
  auto stride = f.stride();

  for (int i = 0; i < nr; ++i)
  {
    for (int j = 0; j < nc; ++j)
      count += lineptr[j] < v;
    lineptr += stride;
  }
  return count;
}

unsigned threshold1_bis(const image2d<uint8>& f, uint8 v)
{

  int nr = f.nrows();
  int nc = f.ncols();
  const mln::uint8* lineptr = f.buffer();
  const auto stride = f.stride();
  unsigned count = 0;

  for (int i = 0; i < nr; ++i)
  {
    int remain = nc;
    auto bucket_buffer = lineptr;
    while (remain > 0)
    {
      uint8 localcount = 0;
      for (int k = 0; k < std::min(remain, 256); ++k)
        localcount += bucket_buffer[k] < v;
      count += localcount;
      remain -= 256;
      bucket_buffer += 256;
    }
    lineptr += stride;
  }
  return count;
}

unsigned threshold2(const image2d<uint8>& f, uint8 v)
{
  unsigned count = 0;
  mln_foreach (auto val, f.values())
  {
    count += (val < v);
  }
  return count;
}

unsigned threshold3(const image2d<uint8>& f, uint8 v)
{
  unsigned count = 0;
  mln_foreach (auto px, f.pixels())
  {
    count += (px.val() < v);
  }
  return count;
}

unsigned threshold4(const image2d<uint8>& f, uint8 v)
{
  return accumulate(f < v, accu::accumulators::sum<unsigned>());
}

void threshold5(const image2d<uint8>& f, image2d<bool>& out, uint8 v)
{
  const int nr = f.nrows();
  const int nc = f.ncols();
  const auto istride = f.stride();
  const auto ostride = out.stride();

  const mln::uint8* i_lineptr = f.buffer();
  bool* o_lineptr = out.buffer();

  for (int i = 0; i < nr; ++i)
  {
    for (int j = 0; j < nc; ++j)
      o_lineptr[j] = i_lineptr[j] < v;

    i_lineptr += istride;
    o_lineptr += ostride;
  }
}

void threshold6(const image2d<uint8>& f, image2d<bool>& out, uint8 v)
{
  copy(f < v, out);
}
