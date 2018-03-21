#include <mln/core/image/image2d.hpp>

#include <vector>

#include <experimental/coroutine>
#include "range/v3/experimental/utility/generator.hpp"
using ranges::experimental::generator;

void Mult_Inplace_Pylene(mln::image2d<mln::uint8>& img)
{
  mln_foreach(auto p, img.pixels())
  {
    p.val() *= 2;
  }
}

void Mult_Inplace_C(std::ptrdiff_t*  info, mln::uint8* buffer)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      buffer[j] *= 2; 
    }
    buffer += stride;
  }
}

void Mult_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_forall(pxIn, pxOut)
  {
    pxOut->val() = pxIn->val() * 2;
  }
}

void Mult_C(std::ptrdiff_t* info, mln::uint8* buffer, mln::uint8* buffer_new)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      buffer_new[j] = buffer[j] * 2; 
    }
    buffer += stride;
    buffer_new += stride;
  }
}

void Threshold_Inplace_Pylene(mln::image2d<mln::uint8>& img)
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

void Threshold_Inplace_C(std::ptrdiff_t* info, mln::uint8* buffer)
{
  mln::uint8 threshold = ~0 >> 1;
  mln::uint8 maxi = ~0;
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
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

void Threshold_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  mln::uint8 threshold = ~0 >> 1;
  mln::uint8 maxi = ~0;
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_forall(pxIn, pxOut)
  {
    if (pxIn->val() < threshold)
    {
      pxOut->val() = 0;
    }
    else
    {
      pxOut->val() = maxi;
    }
  }
}

void Threshold_C(std::ptrdiff_t* info, mln::uint8* buffer, mln::uint8* buffer_new)
{
  mln::uint8 threshold = ~0 >> 1;
  mln::uint8 maxi = ~0;
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      if (buffer[j] < threshold)
      {
        buffer_new[j] = 0;
      }
      else
      {
        buffer_new[j] = maxi;
      }
    }
    buffer += stride;
    buffer_new += stride;
  }
}

void LUT_Inplace_Pylene(mln::image2d<mln::uint8>& img, std::vector<mln::uint8>& LUT)
{
  mln_pixter(px, img)
  mln_forall(px)
  {
    px->val() = LUT[px->val()];
  }
}

void LUT_Inplace_C(std::ptrdiff_t* info, mln::uint8* __restrict buffer, mln::uint8* __restrict LUT)
{
  const int nrow = info[0];
  const int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      buffer[j] = LUT[buffer[j]];
    }
    buffer += stride;
  }
}

void LUT_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img, std::vector<mln::uint8>& LUT)
{
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_forall(pxIn, pxOut)
  {
    pxOut->val() = LUT[pxIn->val()];
  }
}

void LUT_C(std::ptrdiff_t* info, mln::uint8* __restrict buffer, mln::uint8* __restrict buffer_new, mln::uint8* LUT)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      buffer_new[j] = LUT[buffer[j]];
    }
    buffer += stride;
    buffer_new += stride;
  }
}

generator<mln::uint8*> iter_array(mln::uint8* array, int width, int height, std::ptrdiff_t stride)
{ 
  for (int y = 0; y < height; y++)
  { 
    for (int x = 0; x < width; x++)
    { 
      co_yield array + x;
    }
    array += stride;
  }
}

void Mult_Inplace_Cor(std::ptrdiff_t* info, mln::uint8* buffer)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  auto gen = iter_array(buffer, ncol, nrow, stride);
  for (auto p : gen)
  {
    *p *= 2;
  }
}

void LUT_Inplace_Cor(std::ptrdiff_t* info, mln::uint8* __restrict buffer, mln::uint8* __restrict LUT)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  for (auto p : iter_array(buffer, ncol, nrow, stride))
  {
    *p = LUT[*p];
  }
}