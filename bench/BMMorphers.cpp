#include <mln/accu/accumulators/sum.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/utils/ptroffset.hpp>

using namespace mln;

// Threshold A la C
unsigned threshold1(const image2d<uint8>& f, uint8 v)
{
  using T = uint8;

  int         nr      = f.nrows();
  int         nc      = f.ncols();
  const char* ptr_in  = (const char*)&f(point2d{0, 0});
  unsigned    count   = 0;
  int         istride = (int)(f.strides()[0] - nc * sizeof(T));

  for (int i = 0; i < nr; ++i)
  {
    for (int j = 0; j < nc; ++j)
    {
      count += *(const T*)ptr_in < v;
      ptr_in = ptr_offset(ptr_in, sizeof(T));
    }
    ptr_in = ptr_offset(ptr_in, istride);
  }
  return count;
}

unsigned threshold1_bis(const image2d<uint8>& f, uint8 v)
{
  using T = uint8;

  int         nr      = f.nrows();
  int         nc      = f.ncols();
  const char* ptr_in  = (const char*)&f(point2d{0, 0});
  unsigned    count   = 0;
  int         istride = (int)(f.strides()[0] - nc * sizeof(T));

  for (int i = 0; i < nr; ++i)
  {
    int remain = nc;
    while (remain > 0)
    {
      uint8 localcount = 0;
      for (int k = 0; k < std::min(remain, 256); ++k)
      {
        localcount += *(const T*)ptr_in < v;
        ptr_in = ptr_offset(ptr_in, sizeof(T));
      }
      count += localcount;
      remain -= 256;
    }
    ptr_in = ptr_offset(ptr_in, istride);
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
  using T = uint8;

  int         nr      = f.nrows();
  int         nc      = f.ncols();
  const char* ptr_in  = (const char*)&f(point2d{0, 0});
  char*       ptr_out = (char*)&out(point2d{0, 0});
  int         istride = (int)(f.strides()[0] - nc * sizeof(T));

  for (int i = 0; i < nr; ++i)
  {
    for (int j = 0; j < nc; ++j)
    {
      *ptr_out = *(const T*)ptr_in < v;
      ptr_in   = ptr_offset(ptr_in, sizeof(T));
      ptr_out  = ptr_offset(ptr_out, sizeof(T));
    }
    ptr_in  = ptr_offset(ptr_in, istride);
    ptr_out = ptr_offset(ptr_out, istride);
  }
}

void threshold6(const image2d<uint8>& f, image2d<bool>& out, uint8 v)
{
  copy(f < v, out);
}
