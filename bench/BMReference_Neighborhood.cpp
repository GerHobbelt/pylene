#include <mln/core/image/image2d.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/neighb2d.hpp>
#include <cmath>

void Sum(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  mln_pixter(pxIn, input);
  mln_pixter(pxOut, output);
  mln_iter(nx, mln::c8(pxIn));

  mln_forall(pxIn, pxOut)
  {
    int tmp = 0;
    mln_forall(nx)
      tmp += nx->val();
    pxOut->val() = tmp;
  }
}


void Sum_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height, std::ptrdiff_t stride)
{
  for (int i = 0; i < height; i++)
  {
    std::array<const mln::uint8*, 8> nbhs;
    nbhs[0] = ibuffer + -stride -1;
    nbhs[1] = ibuffer + -stride;
    nbhs[2] = ibuffer + -stride + 1;
    nbhs[3] = ibuffer + -1;
    nbhs[4] = ibuffer + 1;
    nbhs[5] = ibuffer + stride -1;
    nbhs[6] = ibuffer + stride;
    nbhs[7] = ibuffer + stride + 1;

    for (int j = 0; j < width; ++j)
    {
      int tmp = 0;
      for (int k = 0; k < 8; ++k)
        tmp += nbhs[k][j];
      obuffer[j] = tmp;
    }
    ibuffer += stride;
    obuffer += stride;
  }
}

void Average(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  mln_pixter(pxIn, input);
  mln_pixter(pxOut, output);
  mln_iter(nx, mln::c8(pxIn));

  mln_forall(pxIn, pxOut)
  {
    int tmp = 0;
    mln_forall(nx)
      tmp += nx->val();
    pxOut->val() = tmp / 8;
  }
}


void Average_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height, std::ptrdiff_t stride)
{
  std::array<std::ptrdiff_t, 8> offset;
  offset[0] = -stride -1;
  offset[1] = -stride;
  offset[2] = -stride + 1;
  offset[3] = -1;
  offset[4] = 1;
  offset[5] = stride -1;
  offset[6] = stride;
  offset[7] = stride +1;

  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      int tmp = 0;
      for (int k = 0; k < 8; k++)
        tmp += ibuffer[j + offset[k]];
      obuffer[j] = tmp / 8;
    }
    ibuffer += stride;
    obuffer += stride;
  }
}

void Erosion(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  mln_pixter(pxIn, input);
  mln_pixter(pxOut, output);
  mln_iter(nx, mln::c8(pxIn));

  mln_forall(pxIn, pxOut)
  {
    mln::uint8 mini = 255;
    mln_forall(nx)
      if (nx->val() < mini)
        mini = nx->val();
    if (pxIn->val() < mini)
      mini = pxIn->val();
    pxOut->val() = mini;
  }
}

void Erosion_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height, std::ptrdiff_t stride)
{
  for (int i = 0; i < height; i++)
  {
    std::array<const mln::uint8*, 8> nbhs;
    nbhs[0] = ibuffer + -stride -1;
    nbhs[1] = ibuffer + -stride;
    nbhs[2] = ibuffer + -stride + 1;
    nbhs[3] = ibuffer + -1;
    nbhs[4] = ibuffer + 1;
    nbhs[5] = ibuffer + stride -1;
    nbhs[6] = ibuffer + stride;
    nbhs[7] = ibuffer + stride + 1;
    for (int j = 0; j < width; j++)
    {
      mln::uint8 mini = ibuffer[j];
      for (int k = 0; k < 8; k++)
        if (nbhs[k][j] < mini)
          mini = nbhs[k][j]; // std::min prevent gcc vectorization
      obuffer[j] = mini;
    }
    ibuffer += stride;
    obuffer += stride;
  }
}

void Isotropic_Diffusion(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  mln_pixter(pxIn, input);
  mln_pixter(pxOut, output);
  mln_iter(nx, mln::c4(pxIn));

  mln_forall(pxIn, pxOut)
  {
    int tmp = 0;
    mln_forall(nx)
      tmp += nx->val();
    pxOut->val() = pxIn->val() + (tmp - 4 * pxIn->val()) / 8;
  }
}

void Isotropic_Diffusion_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height, std::ptrdiff_t stride)
{
  std::array<std::ptrdiff_t, 4> offset;
  offset[0] = -1 * stride;
  offset[1] = -1;
  offset[2] = 1;
  offset[3] = 1 * stride;

  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      int tmp = 0;
      for (int k = 0; k <= 3; k++)
        tmp += ibuffer[j + offset[k]];
      obuffer[j] = ibuffer[j] + (tmp - 4 * ibuffer[j]) / 8;
    }
    ibuffer += stride;
    obuffer += stride;
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

void Anisotropic_Diffusion(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  mln_pixter(pxIn, input);
  mln_pixter(pxOut, output);
  mln_iter(nx, mln::c4(pxIn));

  mln_forall(pxIn, pxOut)
  {
    float tmp = 0;
    mln::uint8 cached_val = pxIn->val();
    mln_forall(nx)
    {
      float gradient = nx->val() - cached_val;
      tmp += gradient * Flux_Function(gradient);
    }
    pxOut->val() = cached_val + 0.125f * tmp;
  }
}

void Anisotropic_Diffusion_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height, std::ptrdiff_t stride)
{
  std::array<std::ptrdiff_t, 4> offset;
  offset[0] = -1 * stride;
  offset[1] = -1;
  offset[2] = 1;
  offset[3] = 1 * stride;

  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      float tmp = 0;
      mln::uint8 cached_val = ibuffer[j];
      for (int k = 0; k <= 3; k++)
      {
        float gradient = ibuffer[j + offset[k]] - cached_val;
        tmp += gradient * Flux_Function(gradient);
      }
      obuffer[j] = cached_val + 0.125f * tmp;
    }
    ibuffer += stride;
    obuffer += stride;
  }
}
