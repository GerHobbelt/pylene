#include <mln/core/image/image2d.hpp>

void Mult_Inplace_Pylene(mln::image2d<mln::uint8>& img)
{
  mln_foreach(auto p, img.pixels())
  {
    p.val() *= 2;
  }
}

void Mult_Inplace_C(mln::image2d<mln::uint8>& img)
{
  int nrow = img.nrows();
  int ncol = img.ncols();
  std::ptrdiff_t stride = img.strides()[0];
  mln::uint8* buffer = &img.at(0,0);
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      buffer[j] *= 2; 
    }
    buffer += stride;
  }
}

void Threshold_Pylene(mln::image2d<mln::uint8>& img)
{
  mln::uint8 threshold = ~0 >> 1;
  mln::uint8 maxi = ~0;
  mln_foreach(auto p, img.pixels())
  {
    if (p.val() < threshold)
    {
      p.val() = 0;
    }
    else
    {
      p.val() = maxi;
    }
  }
}

void Threshold_C(mln::image2d<mln::uint8>& img)
{
  mln::uint8 threshold = ~0 >> 1;
  mln::uint8 maxi = ~0;
  int nrow = img.nrows();
  int ncol = img.ncols();
  std::ptrdiff_t stride = img.strides()[0];
  mln::uint8* buffer = &img.at(0,0);
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      if (buffer[j] < threshold)
      {
        buffer[j] = 0;
      }
      else
      {
        buffer[j] = maxi;
      }
    }
    buffer += stride;
  }
}