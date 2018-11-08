#include <mln/core/image/image2d.hpp>
#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/reverse.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <vector>


void Mult_Inplace_Reversed(mln::image2d<mln::uint8>& img)
{
  mln_reverse_foreach (auto p, img.pixels())
  {
    p.val() *= 2;
  }
}

void Mult_Inplace_New_Values_Reversed(mln::image2d<mln::uint8>& img)
{
  auto reversed_values = mln::ranges::view::reverse(img.new_values());
  mln_foreach_new (auto& v, reversed_values)
  {
    v *= 2;
  }
}

void Mult_Inplace_New_Pixels_Reversed(mln::image2d<mln::uint8>& img)
{
  auto reversed_pixels = mln::ranges::view::reverse(img.new_pixels());
  mln_foreach_new (auto&& px, reversed_pixels)
  {
    px.val() *= 2;
  }
}

void Mult_Inplace_C_Reversed(mln::uint8* buffer, int width, int height, std::ptrdiff_t stride)
{
  buffer += (width - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      buffer[x] *= 2;
    buffer -= stride;
  }
}

void Mult_Reversed(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out)
{
  mln_rpixter(pxIn, in);
  mln_rpixter(pxOut, out);
  mln_forall (pxIn, pxOut)
  {
    pxOut->val() = pxIn->val() * 2;
  }
}

void Mult_C_Reversed(const mln::uint8* __restrict__ ibuffer, mln::uint8* __restrict__ obuffer, int width, int height,
                     std::ptrdiff_t stride)
{
  ibuffer += (width - 1) * stride;
  obuffer += (width - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      obuffer[x] = ibuffer[x] * 2;
    ibuffer -= stride;
    obuffer -= stride;
  }
}

void Mult_New_Values_Reversed(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  auto zipped_values = mln::ranges::view::zip(mln::ranges::view::reverse(input.new_values()),
                                              mln::ranges::view::reverse(output.new_values()));
  for (auto&& r : zipped_values.rows())
  {
    for (auto&& [in_v, out_v] : r)
    {
      out_v = in_v * 2;
    }
  }
}

void Mult_New_Pixels_Reversed(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  auto zipped_pixels = mln::ranges::view::zip(mln::ranges::view::reverse(input.new_pixels()),
                                              mln::ranges::view::reverse(output.new_pixels()));
  for (auto&& r : zipped_pixels.rows())
  {
    for (auto&& [in_p, out_p] : r)
    {
      out_p.val() = in_p.val() * 2;
    }
  }
}

void Threshold_Inplace_Reversed(mln::image2d<mln::uint8>& img)
{
  constexpr mln::uint8 t = 128;

  mln_reverse_foreach (auto p, img.pixels())
  {
    p.val() = p.val() < t ? 0 : 255;
  }
}

void Threshold_Inplace_New_Values_Reversed(mln::image2d<mln::uint8>& img)
{
  constexpr mln::uint8 t = 128;

  auto reversed_values = mln::ranges::view::reverse(img.new_values());
  mln_foreach_new (auto& v, reversed_values)
  {
    v = v < t ? 0 : 255;
  }
}

void Threshold_Inplace_New_Pixels_Reversed(mln::image2d<mln::uint8>& img)
{
  constexpr mln::uint8 t = 128;

  auto reversed_pixels = mln::ranges::view::reverse(img.new_pixels());
  mln_foreach_new (auto&& px, reversed_pixels)
  {
    px.val() = px.val() < t ? 0 : 255;
  }
}

void Threshold_Inplace_C_Reversed(mln::uint8* buffer, int width, int height, std::ptrdiff_t stride)
{
  constexpr mln::uint8 t = 128;

  buffer += (width - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      buffer[x] = buffer[x] < t ? 0 : 255;
    buffer -= stride;
  }
}

void Threshold_C_Reversed(const mln::uint8* __restrict__ ibuffer, mln::uint8* __restrict__ obuffer, int width,
                          int height, std::ptrdiff_t stride)
{
  constexpr mln::uint8 t = 128;

  ibuffer += (width - 1) * stride;
  obuffer += (width - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      obuffer[x] = ibuffer[x] < t ? 0 : 255;
    ibuffer -= stride;
    obuffer -= stride;
  }
}

void Threshold_Reversed(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out)
{
  constexpr mln::uint8 t = 128;
  mln_rpixter(pxIn, in);
  mln_rpixter(pxOut, out);
  mln_forall (pxIn, pxOut)
  {
    pxOut->val() = pxIn->val() < t ? 0 : 255;
  }
}

void Threshold_New_Values_Reversed(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  constexpr mln::uint8 t = 128;

  auto zipped_values = mln::ranges::view::zip(mln::ranges::view::reverse(input.new_values()),
                                              mln::ranges::view::reverse(output.new_values()));
  for (auto&& r : zipped_values.rows())
  {
    for (auto&& [in_v, out_v] : r)
    {
      out_v = in_v < t ? 0 : 255;
    }
  }
}

void Threshold_New_Pixels_Reversed(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  constexpr mln::uint8 t = 128;

  auto zipped_pixels = mln::ranges::view::zip(mln::ranges::view::reverse(input.new_pixels()),
                                              mln::ranges::view::reverse(output.new_pixels()));
  for (auto&& r : zipped_pixels.rows())
  {
    for (auto&& [in_p, out_p] : r)
    {
      out_p.val() = in_p.val() < t ? 0 : 255;
    }
  }
}

void LUT_Inplace_Reversed(const mln::uint8 LUT[], mln::image2d<mln::uint8>& img)
{
  mln_rpixter(px, img);
  mln_forall (px)
  {
    px->val() = LUT[px->val()];
  }
}

void LUT_Inplace_New_Values_Reversed(const mln::uint8 LUT[], mln::image2d<mln::uint8>& img)
{
  mln_foreach_new (auto& v, img.new_values())
  {
    v = LUT[v];
  }
}

void LUT_Inplace_New_Pixels_Reversed(const mln::uint8 LUT[], mln::image2d<mln::uint8>& img)
{
  mln_foreach_new (auto&& px, img.new_pixels())
  {
    px.val() = LUT[px.val()];
  }
}

void LUT_Inplace_C_Reversed(const mln::uint8* LUT, mln::uint8* buffer, int width, int height, std::ptrdiff_t stride)
{
  buffer += (width - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      buffer[x] = LUT[buffer[x]];
    buffer -= stride;
  }
}

void LUT_Reversed(const mln::uint8 LUT[], const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output)
{
  mln_rpixter(pxIn, input);
  mln_rpixter(pxOut, output);
  mln_forall (pxIn, pxOut)
  {
    pxOut->val() = LUT[pxIn->val()];
  }
}

void LUT_C_Reversed(const mln::uint8* LUT, const mln::uint8* __restrict__ ibuffer, mln::uint8* __restrict__ obuffer,
                    int width, int height, std::ptrdiff_t stride)
{
  ibuffer += (width - 1) * stride;
  obuffer += (width - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      obuffer[x] = LUT[ibuffer[x]];
    ibuffer -= stride;
    obuffer -= stride;
  }
}

void LUT_New_Values_Reversed(const mln::uint8 LUT[], const mln::image2d<mln::uint8>& input,
                             mln::image2d<mln::uint8>& output)
{
  auto zipped_values = mln::ranges::view::zip(mln::ranges::view::reverse(input.new_values()),
                                              mln::ranges::view::reverse(output.new_values()));
  for (auto&& r : zipped_values.rows())
  {
    for (auto&& [in_v, out_v] : r)
    {
      out_v = LUT[in_v];
    }
  }
}

void LUT_New_Pixels_Reversed(const mln::uint8 LUT[], const mln::image2d<mln::uint8>& input,
                             mln::image2d<mln::uint8>& output)
{
  auto zipped_pixels = mln::ranges::view::zip(mln::ranges::view::reverse(input.new_pixels()),
                                              mln::ranges::view::reverse(output.new_pixels()));
  for (auto&& r : zipped_pixels.rows())
  {
    for (auto&& [in_p, out_p] : r)
    {
      out_p.val() = LUT[in_p.val()];
    }
  }
}
