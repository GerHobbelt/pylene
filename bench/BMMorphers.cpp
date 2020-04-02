
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/accu/accumulators/sum.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/image/view/operators.hpp>



// Threshold A la C
unsigned threshold1(mln::experimental::image2d<uint8_t> f, uint8_t v)
{
  int         h       = f.height();
  int         w       = f.width();
  const char* buffer  = reinterpret_cast<const char*>(f.buffer());
  auto        stride  = f.byte_stride();
  unsigned    count   = 0;

  for (int i = 0; i < h; ++i)
  {
    const uint8_t* lineptr = reinterpret_cast<const uint8_t*>(buffer + i * stride);
    for (int j = 0; j < w; ++j)
      count += lineptr[j] < v;
  }
  return count;
}

unsigned threshold1_bis(mln::experimental::image2d<uint8_t> f, uint8_t v)
{
  int         h       = f.height();
  int         w       = f.width();
  const char* buffer  = reinterpret_cast<const char*>(f.buffer());
  auto        stride  = f.byte_stride();
  unsigned    count   = 0;

  for (int i = 0; i < h; ++i)
  {
    const uint8_t* lineptr = reinterpret_cast<const uint8_t*>(buffer + i * stride);
    int remain = w;
    while (remain > 0)
    {
      uint8_t localcount = 0;
      for (int k = 0; k < std::min(remain, 256); ++k)
        localcount += lineptr[k] < v;
      count += localcount;
      remain -= 256;
      lineptr += 256;
    }
  }
  return count;
}

unsigned threshold2(mln::experimental::image2d<uint8_t> f, uint8_t v)
{
  unsigned count = 0;
  mln_foreach_new (auto val, f.new_values())
    count += (val < v);

  return count;
}

unsigned threshold3(mln::experimental::image2d<uint8_t> f, uint8_t v)
{
  unsigned count = 0;
  mln_foreach_new (auto px, f.new_pixels())
    count += (px.val() < v);
  return count;
}

unsigned threshold4(mln::experimental::image2d<uint8_t> f, uint8_t v)
{
  using namespace mln::view::ops;

  return mln::accumulate(f < v, mln::accu::accumulators::sum<unsigned>());
}

void threshold5(mln::experimental::image2d<uint8_t> f, mln::experimental::image2d<bool> out, uint8_t v)
{
  const int h = f.height();
  const int w = f.width();

  const char* ibuffer = (const char*)f.buffer();
  char*       obuffer = (char*)out.buffer();
  auto        istride = f.byte_stride();
  auto        ostride = out.byte_stride();

  for (int i = 0; i < h; ++i)
  {
    const uint8_t* ilineptr = reinterpret_cast<const uint8_t*>(ibuffer + i * istride);
    uint8_t* olineptr = reinterpret_cast<uint8_t*>(obuffer + i * ostride);
    for (int j = 0; j < w; ++j)
      olineptr[j] = ilineptr[j] < v;
  }
}

void threshold6(mln::experimental::image2d<uint8_t> f, mln::experimental::image2d<bool> out, uint8_t v)
{
  using namespace mln::view::ops;
  mln::experimental::copy(f < v, out);
}
