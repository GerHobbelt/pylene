#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/reverse.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <vector>



void Mult_Inplace_New_Values_Reversed(mln::experimental::image2d<uint8_t>& img)
{
  auto reversed_values = mln::ranges::view::reverse(img.new_values());
  mln_foreach_new (auto& v, reversed_values)
  {
    v *= 2;
  }
}

void Mult_Inplace_New_Pixels_Reversed(mln::experimental::image2d<uint8_t>& img)
{
  auto reversed_pixels = mln::ranges::view::reverse(img.new_pixels());
  mln_foreach_new (auto&& px, reversed_pixels)
  {
    px.val() *= 2;
  }
}

void Mult_Inplace_C_Reversed(uint8_t* buffer, int width, int height, std::ptrdiff_t stride)
{
  buffer += (height - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      buffer[x] *= 2;
    buffer -= stride;
  }
}


void Mult_C_Reversed(const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width, int height,
                     std::ptrdiff_t stride)
{
  ibuffer += (height - 1) * stride;
  obuffer += (height - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      obuffer[x] = ibuffer[x] * 2;
    ibuffer -= stride;
    obuffer -= stride;
  }
}

void Mult_New_Values_Reversed(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output)
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

void Mult_New_Pixels_Reversed(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output)
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


void Threshold_Inplace_New_Values_Reversed(mln::experimental::image2d<uint8_t>& img)
{
  constexpr uint8_t t = 128;

  auto reversed_values = mln::ranges::view::reverse(img.new_values());
  mln_foreach_new (auto& v, reversed_values)
  {
    v = v < t ? 0 : 255;
  }
}

void Threshold_Inplace_New_Pixels_Reversed(mln::experimental::image2d<uint8_t>& img)
{
  constexpr uint8_t t = 128;

  auto reversed_pixels = mln::ranges::view::reverse(img.new_pixels());
  mln_foreach_new (auto&& px, reversed_pixels)
  {
    px.val() = px.val() < t ? 0 : 255;
  }
}

void Threshold_Inplace_C_Reversed(uint8_t* buffer, int width, int height, std::ptrdiff_t stride)
{
  constexpr uint8_t t = 128;

  buffer += (height - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      buffer[x] = buffer[x] < t ? 0 : 255;
    buffer -= stride;
  }
}

void Threshold_C_Reversed(const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer, int width,
                          int height, std::ptrdiff_t stride)
{
  constexpr uint8_t t = 128;

  ibuffer += (height - 1) * stride;
  obuffer += (height - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      obuffer[x] = ibuffer[x] < t ? 0 : 255;
    ibuffer -= stride;
    obuffer -= stride;
  }
}


void Threshold_New_Values_Reversed(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output)
{
  constexpr uint8_t t = 128;

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

void Threshold_New_Pixels_Reversed(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output)
{
  constexpr uint8_t t = 128;

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


void LUT_Inplace_New_Values_Reversed(const uint8_t LUT[], mln::experimental::image2d<uint8_t>& img)
{
  mln_foreach_new (auto& v, img.new_values())
  {
    v = LUT[v];
  }
}

void LUT_Inplace_New_Pixels_Reversed(const uint8_t LUT[], mln::experimental::image2d<uint8_t>& img)
{
  mln_foreach_new (auto&& px, img.new_pixels())
  {
    px.val() = LUT[px.val()];
  }
}

void LUT_Inplace_C_Reversed(const uint8_t* LUT, uint8_t* buffer, int width, int height, std::ptrdiff_t stride)
{
  buffer += (height - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      buffer[x] = LUT[buffer[x]];
    buffer -= stride;
  }
}


void LUT_C_Reversed(const uint8_t* LUT, const uint8_t* __restrict__ ibuffer, uint8_t* __restrict__ obuffer,
                    int width, int height, std::ptrdiff_t stride)
{
  ibuffer += (height - 1) * stride;
  obuffer += (height - 1) * stride;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = width - 1; x >= 0; --x)
      obuffer[x] = LUT[ibuffer[x]];
    ibuffer -= stride;
    obuffer -= stride;
  }
}

void LUT_New_Values_Reversed(const uint8_t LUT[], const mln::experimental::image2d<uint8_t>& input,
                             mln::experimental::image2d<uint8_t>& output)
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

void LUT_New_Pixels_Reversed(const uint8_t LUT[], const mln::experimental::image2d<uint8_t>& input,
                             mln::experimental::image2d<uint8_t>& output)
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
