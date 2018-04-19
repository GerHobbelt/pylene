#include <mln/core/image/image2d.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/neighb2d.hpp>

#include <cmath>

#include "range2d.hpp"
#include "win3x3.hpp"

void Sum_Pylene(const mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_iter(n, mln::c8(pxIn));

  mln_forall(pxIn, pxOut)
  {
    int tmp = 0;
    mln_forall(n)
    {
      tmp += n->val();
    }
    pxOut->val() = tmp;
  }
}

void Sum_New(const mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  auto rngIn = pixels_of(img);
  auto rngOut = pixels_of(new_img);

  mln_foreach2((auto [pxIn, pxOut]), ranges::view::zip(rngIn, rngOut))
  {
    pxOut.val() = 0;
    for(auto nx : win3x3(pxIn))
      pxOut.val() += nx.val();
  }
}

void Sum_C(int* info, mln::uint8* __restrict buffer, mln::uint8* __restrict buffer_new)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];
  for (int i = 0; i < nrow; i++)
  {
    std::array<const mln::uint8*, 8> nbhs;
    nbhs[0] = buffer + -stride -1;
    nbhs[1] = buffer + -stride;
    nbhs[2] = buffer + -stride + 1;
    nbhs[3] = buffer + -1;
    nbhs[4] = buffer + 1;
    nbhs[5] = buffer + stride -1;
    nbhs[6] = buffer + stride;
    nbhs[7] = buffer + stride + 1;

    for (int j = 0; j < ncol; ++j)
    {
      mln::uint8 tmp = 0;
      for (int k = 0; k < 8; ++k)
        tmp += nbhs[k][j];
      buffer_new[j] = tmp;
    }
    buffer += stride;
    buffer_new += stride;
  }
}

void Average_Pylene(const mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_iter(n, mln::c8(pxIn));

  mln_forall(pxIn, pxOut)
  {
    int tmp = 0;
    mln_forall(n)
    {
      tmp += n->val();
    }
    pxOut->val() = tmp / 8;
  }
}

void Average_New(const mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  auto rngIn = pixels_of(img);
  auto rngOut = pixels_of(new_img);

  mln_foreach2((auto [pxIn, pxOut]), ranges::view::zip(rngIn, rngOut))
  {
    pxOut.val() = 0;
    for(auto nx : win3x3(pxIn))
      pxOut.val() += nx.val();
    pxOut.val() /= 8;
  }
}

void Average_C(int* info, mln::uint8* buffer, mln::uint8* buffer_new)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];

  std::array<std::ptrdiff_t, 8> offset;
  offset[0] = -stride -1;
  offset[1] = -stride;
  offset[2] = -stride + 1;
  offset[3] = -1;
  offset[4] = 1;
  offset[5] = stride -1;
  offset[6] = stride;
  offset[7] = stride +1;

  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      buffer_new[j] = 0;
      for (int k = 0; k < 8; k++)
        buffer_new[j] += buffer[j + offset[k]];
      buffer_new[j] /= 8;
    }
    buffer += stride;
    buffer_new += stride;
  }
}

void Erosion_Pylene(const mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_iter(n, mln::c8(pxIn));

  mln_forall(pxIn, pxOut)
  {
    mln::uint8 mini = 255;
    mln_forall(n)
      if (n->val() < mini)
        mini = n->val();
    if (pxIn->val() < mini)
      mini = pxIn->val();
    pxOut->val() = mini;
  }
}

void Erosion_New(const mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  auto rngIn = pixels_of(img);
  auto rngOut = pixels_of(new_img);

  mln_foreach2((auto [pxIn, pxOut]), ranges::view::zip(rngIn, rngOut))
  {
    mln::uint8 tmp = pxIn.val();
    for(auto nx : win3x3(pxIn))
      if (nx.val() < tmp)
        tmp = nx.val();
    pxOut.val() = tmp;
  }
}

void Erosion_C(int* info, mln::uint8* __restrict__ buffer, mln::uint8* __restrict__ buffer_new)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];


  for (int i = 0; i < nrow; i++)
  {
    std::array<const mln::uint8*, 8> nbhs;
    nbhs[0] = buffer + -stride -1;
    nbhs[1] = buffer + -stride;
    nbhs[2] = buffer + -stride + 1;
    nbhs[3] = buffer + -1;
    nbhs[4] = buffer + 1;
    nbhs[5] = buffer + stride -1;
    nbhs[6] = buffer + stride;
    nbhs[7] = buffer + stride + 1;
    for (int j = 0; j < ncol; j++)
    {
      mln::uint8 mini = buffer[j];
      for (int k = 0; k < 8; k++)
        if (nbhs[k][j] < mini)
          mini = nbhs[k][j]; // std::min prevent gcc vectorization
      buffer_new[j] = mini;
    }
    buffer += stride;
    buffer_new += stride;
  }
}

void Isotropic_Diffusion_Pylene(const mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_iter(n, mln::c4(pxIn));

  mln_forall(pxIn, pxOut)
  {
    int tmp = 0;
    mln_forall(n)
    {
      tmp += n->val();
    }
    pxOut->val() = pxIn->val() + (tmp - 4 * pxIn->val()) / 8;
  }
}

void Isotropic_Diffusion_New(const mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{

  auto rngIn = pixels_of(img);
  auto rngOut = pixels_of(new_img);

  mln_foreach2((auto [pxIn, pxOut]), ranges::view::zip(rngIn, rngOut))
  {
    int tmp = 0;
    for(auto nx : c4(pxIn))
      tmp += nx.val();
    pxOut.val() = pxIn.val() + (tmp - 4 * pxIn.val()) / 8;
  }
}

void Isotropic_Diffusion_C(int* info, mln::uint8* __restrict__ buffer, mln::uint8* __restrict__ buffer_new)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];

  std::array<std::ptrdiff_t, 4> offset;
  offset[0] = -1 * stride;
  offset[1] = -1;
  offset[2] = 1;
  offset[3] = 1 * stride;

  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      int tmp = 0;
      for (int k = 0; k <= 3; k++)
        tmp += buffer[j + offset[k]];
      buffer_new[j] = buffer[j] + (tmp - 4 * buffer[j]) / 8;
    }
    buffer += stride;
    buffer_new += stride;
  }
}

// Here Quadratic flux function is used for anisotropic diffusion
// The constant 0.35 (K) determines the edge-strenght to consider
// as a valid region boundary, it is a free parameter.
constexpr float Flux_Function(float gradient)
{
  float tmp = gradient / 0.35f;
  return 1.0f / (1.0f + tmp * tmp);
}

void Anisotropic_Diffusion_Pylene(const mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{
  mln_pixter(pxIn, img);
  mln_pixter(pxOut, new_img);
  mln_iter(n, mln::c4(pxIn));

  mln_forall(pxIn, pxOut)
  {
    float tmp = 0;
    mln::uint8 cached_val = pxIn->val();
    mln_forall(n)
    {
      float gradient = n->val() - cached_val;
      tmp += gradient * Flux_Function(gradient);
    }
    pxOut->val() = cached_val + 0.125 * tmp;
  }
}

void Anisotropic_Diffusion_New(const mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img)
{

  auto rngIn = pixels_of(img);
  auto rngOut = pixels_of(new_img);

  mln_foreach2((auto [pxIn, pxOut]), ranges::view::zip(rngIn, rngOut))
  {
    float tmp = 0;
    auto vin = pxIn.val();
    for(auto nx : c4(pxIn))
    {
      float gradient = nx.val() - vin;
      tmp += gradient * Flux_Function(gradient);
    }
    pxOut.val() = vin + 0.125f * tmp;
  }
}

void Anisotropic_Diffusion_C(int* info, mln::uint8* buffer, mln::uint8* buffer_new)
{
  int nrow = info[0];
  int ncol = info[1];
  std::ptrdiff_t stride = info[2];

  std::array<std::ptrdiff_t, 4> offset;
  offset[0] = -1 * stride;
  offset[1] = -1;
  offset[2] = 1;
  offset[3] = 1 * stride;

  for (int i = 0; i < nrow; i++)
  {
    for (int j = 0; j < ncol; j++)
    {
      float tmp = 0;
      mln::uint8 cached_val = buffer[j];
      for (int k = 0; k <= 3; k++)
      {
        float gradient = buffer[j + offset[k]] - cached_val;
        tmp += gradient * Flux_Function(gradient);
      }
      buffer_new[j] = cached_val + 0.125f * tmp;
    }
    buffer += stride;
    buffer_new += stride;
  }
}
