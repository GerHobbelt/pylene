#include <bench/neighborhood.hpp>

#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <cmath>


void Sum(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  mln_pixter(pxIn, input);
  mln_pixter(pxOut, output);
  mln_iter(nx, mln::c8(pxIn));

  mln_forall (pxIn, pxOut)
  {
    int tmp = 0;
    mln_forall (nx)
      tmp += nx->val();
    pxOut->val() = tmp;
  }
}

void Sum_New(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  auto pixels = mln::ranges::view::zip(input.new_pixels(), output.new_pixels());
  for (auto rows : pixels.rows())
    for ([[maybe_unused]] auto&& [pxIn, pxOut] : rows)
    {
      int tmp = 0;
#ifdef PYLENE_GCC8_WORKAROUND
      (void)pxIn;
      (void)pxOut;
#else
      for (auto nx : mln::experimental::c8(pxIn))
        tmp += nx.val();
#endif
      pxOut.val() = tmp;
    }
}


void Sum_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height,
           std::ptrdiff_t stride)
{
  for (int i = 0; i < height; i++)
  {
    std::array<const mln::uint8*, 8> nbhs;
    nbhs[0] = ibuffer + -stride - 1;
    nbhs[1] = ibuffer + -stride;
    nbhs[2] = ibuffer + -stride + 1;
    nbhs[3] = ibuffer + -1;
    nbhs[4] = ibuffer + 1;
    nbhs[5] = ibuffer + stride - 1;
    nbhs[6] = ibuffer + stride;
    nbhs[7] = ibuffer + stride + 1;

    for (int j = 0; j < width; ++j)
    {
      int tmp = 0;
      tmp += nbhs[0][j];
      tmp += nbhs[1][j];
      tmp += nbhs[2][j];
      tmp += nbhs[3][j];
      tmp += nbhs[4][j];
      tmp += nbhs[5][j];
      tmp += nbhs[6][j];
      tmp += nbhs[7][j];
      obuffer[j] = tmp;
    }
    ibuffer += stride;
    obuffer += stride;
  }
}

void Average_New(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  auto pixels = mln::ranges::view::zip(input.new_pixels(), output.new_pixels());
  for (auto rows : pixels.rows())
    for (auto&& [pxIn, pxOut] : rows)
    {
      int tmp = 0;
      for (auto nx : mln::experimental::c8(pxIn))
        tmp += nx.val();
      pxOut.val() = tmp / 8;
    }
}


void Average(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  mln_pixter(pxIn, input);
  mln_pixter(pxOut, output);
  mln_iter(nx, mln::c8(pxIn));

  mln_forall (pxIn, pxOut)
  {
    int tmp = 0;
    mln_forall (nx)
      tmp += nx->val();
    pxOut->val() = tmp / 8;
  }
}


void Average_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height,
               std::ptrdiff_t stride)
{
  std::array<std::ptrdiff_t, 8> offset;
  offset[0] = -stride - 1;
  offset[1] = -stride;
  offset[2] = -stride + 1;
  offset[3] = -1;
  offset[4] = 1;
  offset[5] = stride - 1;
  offset[6] = stride;
  offset[7] = stride + 1;

  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      int tmp = 0;
      tmp += ibuffer[j + offset[0]];
      tmp += ibuffer[j + offset[1]];
      tmp += ibuffer[j + offset[2]];
      tmp += ibuffer[j + offset[3]];
      tmp += ibuffer[j + offset[4]];
      tmp += ibuffer[j + offset[5]];
      tmp += ibuffer[j + offset[6]];
      tmp += ibuffer[j + offset[7]];
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

  mln_forall (pxIn, pxOut)
  {
    mln::uint8 mini = 255;
    mln_forall (nx)
      if (nx->val() < mini)
        mini = nx->val();
    if (pxIn->val() < mini)
      mini = pxIn->val();
    pxOut->val() = mini;
  }
}

void Erosion_New(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  auto pixels = mln::ranges::view::zip(input.new_pixels(), output.new_pixels());
  for (auto rows : pixels.rows())
    for (auto&& [pxIn, pxOut] : rows)
    {
      mln::uint8 mini = pxIn.val();
      for (auto nx : mln::experimental::c8(pxIn))
        if (nx.val() < mini)
          mini = nx.val();
      pxOut.val() = mini;
    }
}

void Erosion_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height,
               std::ptrdiff_t stride)
{
  for (int i = 0; i < height; i++)
  {
    std::array<const mln::uint8*, 8> nbhs;
    nbhs[0] = ibuffer + -stride - 1;
    nbhs[1] = ibuffer + -stride;
    nbhs[2] = ibuffer + -stride + 1;
    nbhs[3] = ibuffer + -1;
    nbhs[4] = ibuffer + 1;
    nbhs[5] = ibuffer + stride - 1;
    nbhs[6] = ibuffer + stride;
    nbhs[7] = ibuffer + stride + 1;
    for (int j = 0; j < width; j++)
    {
      mln::uint8 mini = ibuffer[j];

      if (nbhs[0][j] < mini)
        mini = nbhs[0][j]; // std::min prevent gcc vectorization
      if (nbhs[1][j] < mini)
        mini = nbhs[1][j]; // std::min prevent gcc vectorization
      if (nbhs[2][j] < mini)
        mini = nbhs[2][j]; // std::min prevent gcc vectorization
      if (nbhs[3][j] < mini)
        mini = nbhs[3][j]; // std::min prevent gcc vectorization
      if (nbhs[4][j] < mini)
        mini = nbhs[4][j]; // std::min prevent gcc vectorization
      if (nbhs[5][j] < mini)
        mini = nbhs[5][j]; // std::min prevent gcc vectorization
      if (nbhs[6][j] < mini)
        mini = nbhs[6][j]; // std::min prevent gcc vectorization
      if (nbhs[7][j] < mini)
        mini = nbhs[7][j]; // std::min prevent gcc vectorization

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

  mln_forall (pxIn, pxOut)
  {
    int tmp = 0;
    mln_forall (nx)
      tmp += nx->val();
    pxOut->val() = pxIn->val() + (tmp - 4 * pxIn->val()) / 8;
  }
}

void Isotropic_Diffusion_New(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  auto pixels = mln::ranges::view::zip(input.new_pixels(), output.new_pixels());
  for (auto rows : pixels.rows())
    for (auto&& [pxIn, pxOut] : rows)
    {
      int tmp = 0;
      for (auto nx : mln::experimental::c4(pxIn))
        tmp += nx.val();
      pxOut.val() = pxIn.val() + (tmp - 4 * pxIn.val()) / 8;
    }
}

void Isotropic_Diffusion_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height,
                           std::ptrdiff_t stride)
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
      //#pragma clang loop unroll(full)
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

  mln_forall (pxIn, pxOut)
  {
    float      tmp        = 0;
    mln::uint8 cached_val = pxIn->val();
    mln_forall (nx)
    {
      float gradient = nx->val() - cached_val;
      tmp += gradient * Flux_Function(gradient);
    }
    pxOut->val() = cached_val + 0.125f * tmp;
  }
}

void Anisotropic_Diffusion_New(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  auto pixels = mln::ranges::view::zip(input.new_pixels(), output.new_pixels());
  for (auto rows : pixels.rows())
    for (auto&& [pxIn, pxOut] : rows)
    {
      float      tmp = 0;
      mln::uint8 vin = pxIn.val();
      for (auto nx : mln::experimental::c4(pxIn))
      {
        float gradient = nx.val() - vin;
        tmp += gradient * Flux_Function(gradient);
      }
      pxOut.val() = vin + 0.125f * tmp;
    }
}

void Anisotropic_Diffusion_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width,
                             int height, std::ptrdiff_t stride)
{
  std::array<std::ptrdiff_t, 4> offset;
  offset[0] = -1 * stride;
  offset[1] = -1;
  offset[2] = 1;
  offset[3] = 1 * stride;

  //#pragma clang loop vectorize(disable)
  for (int i = 0; i < height; i++)
  {
    //#pragma clang loop vectorize(enable)
    for (int j = 0; j < width; j++)
    {
      float      tmp        = 0;
      auto       ptr        = ibuffer + j;
      mln::uint8 cached_val = ptr[0];
      for (auto off : offset)
      {
        float gradient = ptr[off] - cached_val;
        tmp += gradient * Flux_Function(gradient);
      }
      obuffer[j] = cached_val + 0.125f * tmp;
    }
    ibuffer += stride;
    obuffer += stride;
  }
}
