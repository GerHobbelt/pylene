#include <bench/neighborhood.hpp>

#include <mln/core/extension/fill.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/image/experimental/ndimage.hpp>

#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/view/zip.hpp>



void Sum(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output)
{
  auto pixels = mln::ranges::view::zip(input.new_pixels(), output.new_pixels());
  for (auto rows : pixels.rows())
    for (auto&& [pxIn, pxOut] : rows)
    {
      int tmp = 0;
      for (auto nx : mln::experimental::c8(pxIn))
        tmp += nx.val();
      pxOut.val() = tmp;
    }
}



void Sum_C(const uint8_t* __restrict ibuffer, uint8_t* __restrict obuffer, int width, int height,
           std::ptrdiff_t stride)
{
  for (int i = 0; i < height; i++)
  {
    std::array<const uint8_t*, 8> nbhs;
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


void Average(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output)
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


void Average_C(const uint8_t* __restrict ibuffer, uint8_t* __restrict obuffer, int width, int height,
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


void Erosion(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output)
{
  auto pixels = mln::ranges::view::zip(input.new_pixels(), output.new_pixels());
  for (auto rows : pixels.rows())
    for (auto&& [pxIn, pxOut] : rows)
    {
      uint8_t mini = pxIn.val();
      for (auto nx : mln::experimental::c8(pxIn))
        if (nx.val() < mini)
          mini = nx.val();
      pxOut.val() = mini;
    }
}


void Erosion_C(const uint8_t* __restrict ibuffer, uint8_t* __restrict obuffer, int width, int height,
               std::ptrdiff_t stride)
{
  for (int i = 0; i < height; i++)
  {
    std::array<const uint8_t*, 8> nbhs;
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
      uint8_t mini = ibuffer[j];

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


void Isotropic_Diffusion(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output)
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


void Isotropic_Diffusion_C(const uint8_t* __restrict ibuffer, uint8_t* __restrict obuffer, int width, int height,
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

void Anisotropic_Diffusion(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output)
{
  auto pixels = mln::ranges::view::zip(input.new_pixels(), output.new_pixels());
  for (auto rows : pixels.rows())
    for (auto&& [pxIn, pxOut] : rows)
    {
      float      tmp = 0;
      uint8_t vin = pxIn.val();
      for (auto nx : mln::experimental::c4(pxIn))
      {
        float gradient = nx.val() - vin;
        tmp += gradient * Flux_Function(gradient);
      }
      pxOut.val() = vin + 0.125f * tmp;
    }
}


void Anisotropic_Diffusion_C(const uint8_t* __restrict ibuffer, uint8_t* __restrict obuffer, int width,
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
      uint8_t    cached_val = ptr[0];
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

