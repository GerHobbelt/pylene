#include <mln/core/image/image2d.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/neighb2d.hpp>

void Sum_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_iter(n, mln::c8(pxIn));

  mln_forall(pxIn, pxOut)
  {
    unsigned int tmp = 0;
    mln_forall(n)
    {
      tmp += n->val();
    }
    pxOut->val() = tmp;
  }
}

void Sum_C(int* info, mln::uint8* buffer, mln::uint8* buffer_new)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      for (int k = -1; k <= 1; k++)
        for (int l = -1; l <= 1; l++)
        {
          if (l || k)
            buffer_new[j] += buffer[j + k * stride + l];
        }
    }
    buffer += stride;
    buffer_new += stride;
  }
}

void Average_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_iter(n, mln::c8(pxIn));

  mln_forall(pxIn, pxOut)
  {
    unsigned int tmp = 0;
    mln_forall(n)
    {
      tmp += n->val();
    }
    pxOut->val() = tmp / 8;
  }
}

void Average_C(int* info, mln::uint8* buffer, mln::uint8* buffer_new)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      for (int k = -1; k <= 1; k++)
        for (int l = -1; l <= 1; l++)
        {
          if (k || l)
            buffer_new[j] += buffer[j + k * stride + l];
        }
      buffer_new[j] /= 8;
    }
    buffer += stride;
    buffer_new += stride;
  }
}

void Erosion_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_iter(n, mln::c8(pxIn));

  mln_forall(pxIn, pxOut)
  {
    mln::uint8 mini = ~0;
    mln_forall(n)
    {
      if (n->val() < mini)
        mini = n->val();
    }
    pxOut->val() = mini;
  }
}

void Erosion_C(int* info, mln::uint8* buffer, mln::uint8* buffer_new)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      mln::uint8 mini = ~0;
      for (int k = -1; k <= 1; k++)
        for (int l = -1; l <= 1; l++)
        {
          if (mini > buffer[j + k * stride + l])
            mini = buffer[j + k * stride + l];
        }
      buffer_new[j] = mini;
    }
    buffer += stride;
    buffer_new += stride;
  } 
}