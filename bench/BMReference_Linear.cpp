#include <mln/core/image/image2d.hpp>
#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <vector>

void Mult_Inplace(mln::image2d<mln::uint8>& img)
{
  mln_foreach (auto p, img.pixels())
    p.val() *= 2;
}

void Mult_Inplace_New_Values(mln::image2d<mln::uint8>& img)
{
  mln_foreach_new(auto& v, img.new_values()) v *= 2;
}

void Mult_Inplace_New_Pixels(mln::image2d<mln::uint8>& img)
{
  mln_foreach_new(auto&& px, img.new_pixels()) px.val() *= 2;
}

void Mult_Inplace_C(mln::uint8* buffer, int width, int height, std::ptrdiff_t stride)
{
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
      buffer[x] *= 2;
    buffer += stride;
  }
}

void Mult(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out)
{
  mln_pixter(pxIn, in);
  mln_pixter(pxOut, out);
  mln_forall (pxIn, pxOut)
    pxOut->val() = pxIn->val() * 2;
}

void Mult_C(const mln::uint8* __restrict__ ibuffer, mln::uint8* __restrict__ obuffer, int width, int height,
            std::ptrdiff_t stride)
{
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
      obuffer[x] = ibuffer[x] * 2;

    ibuffer += stride;
    obuffer += stride;
  }
}

void Mult_New_Values(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out)
{
  auto zipped_values = mln::ranges::view::zip(in.new_values(), out.new_values());
  for (auto&& r : mln::ranges::rows(zipped_values))
  {
    for (auto&& [in_v, out_v] : r)
    {
      out_v = in_v * 2;
    }
  }
}

void Mult_New_Pixels(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out)
{
  auto zipped_pixels = mln::ranges::view::zip(in.new_pixels(), out.new_pixels());
  for (auto&& r : mln::ranges::rows(zipped_pixels))
  {
    for (auto&& [in_p, out_p] : r)
    {
      out_p.val() = in_p.val() * 2;
    }
  }
}


void Threshold_Inplace(mln::image2d<mln::uint8>& img)
{
  constexpr mln::uint8 t = 128;

  mln_foreach (auto p, img.pixels())
    p.val() = p.val() < t ? 0 : 255;
}

void Threshold_Inplace_New_Values(mln::image2d<mln::uint8>& img)
{
  constexpr mln::uint8 t = 128;

  mln_foreach_new(auto& v, img.new_values()) v = v < t ? 0 : 255;
}

void Threshold_Inplace_New_Pixels(mln::image2d<mln::uint8>& img)
{
  constexpr mln::uint8 t = 128;

  mln_foreach_new(auto&& px, img.new_pixels()) px.val() = px.val() < t ? 0 : 255;
}

void Threshold_Inplace_C(mln::uint8* buffer, int width, int height, std::ptrdiff_t stride)
{
  constexpr mln::uint8 t = 128;

  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
      buffer[x] = buffer[x] < t ? 0 : 255;
    buffer += stride;
  }
}

void Threshold_C(const mln::uint8* __restrict__ ibuffer, mln::uint8* __restrict__ obuffer, int width, int height,
                 std::ptrdiff_t stride)
{
  constexpr mln::uint8 t = 128;

  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
      obuffer[x] = ibuffer[x] < t ? 0 : 255;
    ibuffer += stride;
    obuffer += stride;
  }
}

void Threshold(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out)
{
  constexpr mln::uint8 t = 128;
  mln_pixter(pxIn, in);
  mln_pixter(pxOut, out);
  mln_forall (pxIn, pxOut)
    pxOut->val() = pxIn->val() < t ? 0 : 255;
}

void LUT_Inplace(const mln::uint8 LUT[], mln::image2d<mln::uint8>& img)
{
  mln_pixter(px, img) mln_forall (px) { px->val() = LUT[px->val()]; }
}

void LUT_Inplace_New_Values(const mln::uint8 LUT[], mln::image2d<mln::uint8>& img)
{
  mln_foreach_new(auto& v, img.new_values()) v = LUT[v];
}

void LUT_Inplace_New_Pixels(const mln::uint8 LUT[], mln::image2d<mln::uint8>& img)
{
  mln_foreach_new(auto&& px, img.new_pixels()) px.val() = LUT[px.val()];
}

void LUT_Inplace_C(const mln::uint8* LUT, mln::uint8* buffer, int width, int height, std::ptrdiff_t stride)
{
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
      buffer[x] = LUT[buffer[x]];
    buffer += stride;
  }
}

void LUT(const mln::uint8 LUT[], const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  mln_pixter(pxIn, input);
  mln_pixter(pxOut, output);
  mln_forall (pxIn, pxOut)
  {
    pxOut->val() = LUT[pxIn->val()];
  }
}

void LUT_C(const mln::uint8* LUT, const mln::uint8* __restrict__ ibuffer, mln::uint8* __restrict__ obuffer, int width,
           int height, std::ptrdiff_t stride)
{
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
      obuffer[x] = LUT[ibuffer[x]];
    ibuffer += stride;
    obuffer += stride;
  }
}
