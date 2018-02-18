#include <mln/core/image/image2d.hpp>
#include <mln/core/extension/fill.hpp>

void Sum_Pylene(mln::image2d<mln::uint8>& img)
{
  mln::image2d<mln::uint8> new_img(0,0);
  mln::resize(new_img, img);
  mln::extension::fill(img, 0);
  mln_foreach(auto p, img.domain())
  {
    unsigned tmp = 0;
    for (int i = -1; i <= 1; i++)
      for (int j = -1; j <= 1; j++)
      {
        mln::point2d p2(i,j);
        tmp += img(p + p2);
      }
    new_img(p) = tmp;
  }
}

void Sum_C(mln::image2d<mln::uint8>& img)
{
  mln::image2d<mln::uint8> new_img(0,0);
  mln::resize(new_img, img);
  mln::extension::fill(img, 0);

  int nrow = img.nrows();
  int ncol = img.ncols();
  std::ptrdiff_t stride = img.strides()[0];

  mln::uint8* buffer = &img.at(0,0);
  mln::uint8* buffer_new = &new_img.at(0,0);
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      for (int k = -1; k <= 1; k++)
        for (int l = -1; l <= 1; l++)
        {
          buffer_new[j] += buffer[j + k * stride + l];
        }
    }
    buffer += stride;
    buffer_new += stride;
  }
}

void Average_Pylene(mln::image2d<mln::uint8>& img)
{
  mln::image2d<mln::uint8> new_img(0,0);
  mln::resize(new_img, img);
  mln::extension::fill(img, 0);
  mln_foreach(auto p, img.domain())
  {
    unsigned int tmp = 0;
    for (int i = -1; i <= 1; i++)
      for (int j = -1; j <= 1; j++)
      {
        mln::point2d p2(i,j);
        tmp += img(p + p2);
      }
    new_img(p) = tmp / 9;
  }
}

void Average_C(mln::image2d<mln::uint8>& img)
{
  mln::image2d<mln::uint8> new_img(0,0);
  mln::resize(new_img, img);
  mln::extension::fill(img, 0);

  int nrow = img.nrows();
  int ncol = img.ncols();
  std::ptrdiff_t stride = img.strides()[0];

  mln::uint8* buffer = &img.at(0,0);
  mln::uint8* buffer_new = &new_img.at(0,0);
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      unsigned int tmp = 0;
      for (int k = -1; k <= 1; k++)
        for (int l = -1; l <= 1; l++)
        {
          tmp += buffer[j + k * stride + l];
        }
      buffer_new[j] = tmp / 9;
    }
    buffer += stride;
    buffer_new += stride;
  }
}